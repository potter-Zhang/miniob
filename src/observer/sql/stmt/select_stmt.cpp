/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2022/6/6.
//

#include "sql/stmt/select_stmt.h"
#include "sql/stmt/filter_stmt.h"
#include "common/log/log.h"
#include "common/lang/string.h"
#include "storage/db/db.h"
#include "storage/table/table.h"

SelectStmt::~SelectStmt()
{
  if (nullptr != filter_stmt_) {
    delete filter_stmt_;
    filter_stmt_ = nullptr;
  }
}

static void wildcard_fields(Table *table, std::vector<Field> &field_metas)
{
  const TableMeta &table_meta = table->table_meta();
  const int field_num = table_meta.field_num();
  for (int i = table_meta.sys_field_num(); i < field_num; i++) {
    field_metas.push_back(Field(table, table_meta.field(i)));
  }
}

RC SelectStmt::create(Db *db, const SelectSqlNode &select_sql, Stmt *&stmt)
{
  if (nullptr == db) {
    LOG_WARN("invalid argument. db is null");
    return RC::INVALID_ARGUMENT;
  }

  // collect tables in `from` statement
  std::vector<Table *> tables;
  std::unordered_map<std::string, Table *> table_map;
  for (size_t i = 0; i < select_sql.relations.size(); i++) {
    const char *table_name = select_sql.relations[i].c_str();
    if (nullptr == table_name) {
      LOG_WARN("invalid argument. relation name is null. index=%d", i);
      return RC::INVALID_ARGUMENT;
    }

    Table *table = db->find_table(table_name);
    if (nullptr == table) {
      LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
      return RC::SCHEMA_TABLE_NOT_EXIST;
    }

    tables.push_back(table);
    table_map.insert(std::pair<std::string, Table *>(table_name, table));
  }

  // collect query fields in `select` statement
  std::vector<Field> query_fields;
  for (int i = static_cast<int>(select_sql.attributes.size()) - 1; i >= 0; i--) {
    const RelAttrSqlNode &relation_attr = select_sql.attributes[i];
    int length = query_fields.size();
    if (common::is_blank(relation_attr.relation_name.c_str()) &&
        0 == strcmp(relation_attr.attribute_name.c_str(), "*")) {
      // 看有没有聚合查询
      if (relation_attr.func != NONE){
        if (relation_attr.func != AggregationFunc::COUNTFUN){
          LOG_WARN("invalid aggregation query: AggregationFunc(%d) of *", relation_attr.func);
          return RC::SQL_SYNTAX;
        }
        for (Table *table : tables){
          const TableMeta &table_meta = table->table_meta();
          const int field_num = table_meta.field_num();
          query_fields.push_back(Field(table, table_meta.field(table_meta.sys_field_num())));
          query_fields.back().set_is_star(true);
        }
        for (auto iter = query_fields.begin() + length; iter != query_fields.end(); iter++)
          iter->set_func(relation_attr.func);
      }
      else{
        for (Table *table : tables) {
          wildcard_fields(table, query_fields);
        }     
      }
    } else if (!common::is_blank(relation_attr.relation_name.c_str())) {      
      const char *table_name = relation_attr.relation_name.c_str();
      const char *field_name = relation_attr.attribute_name.c_str();

      if (0 == strcmp(table_name, "*")) {
        if (0 != strcmp(field_name, "*")) {
          LOG_WARN("invalid field name while table is *. attr=%s", field_name);
          return RC::SCHEMA_FIELD_MISSING;
        }
        // 看有没有聚合查询
        if (relation_attr.func != NONE){
          if (relation_attr.func != AggregationFunc::COUNTFUN){
            LOG_WARN("invalid aggregation query: AggregationFunc(%d) of *", relation_attr.func);
            return RC::SQL_SYNTAX;
          }
          for (Table *table : tables) {
            wildcard_fields(table, query_fields);
          }
          for (auto iter = query_fields.begin() + length; iter != query_fields.end(); iter++)
            iter->set_func(relation_attr.func);
        }
      } else {
        auto iter = table_map.find(table_name);
        if (iter == table_map.end()) {
          LOG_WARN("no such table in from list: %s", table_name);
          return RC::SCHEMA_FIELD_MISSING;
        }

        Table *table = iter->second;
        if (0 == strcmp(field_name, "*")) {
          wildcard_fields(table, query_fields);
        } else {
          const FieldMeta *field_meta = table->table_meta().field(field_name);
          if (nullptr == field_meta) {
            LOG_WARN("no such field. field=%s.%s.%s", db->name(), table->name(), field_name);
            return RC::SCHEMA_FIELD_MISSING;
          }

          query_fields.push_back(Field(table, field_meta));
          // 看有没有聚合查询
          if (relation_attr.func != NONE){
            for (auto iter = query_fields.begin() + length; iter != query_fields.end(); iter++)
              iter->set_func(relation_attr.func);
          }
        }
      }
    } else {
      if (tables.size() != 1) {
        LOG_WARN("invalid. I do not know the attr's table. attr=%s", relation_attr.attribute_name.c_str());
        return RC::SCHEMA_FIELD_MISSING;
      }

      Table *table = tables[0];
      const FieldMeta *field_meta = table->table_meta().field(relation_attr.attribute_name.c_str());
      if (nullptr == field_meta) {
        LOG_WARN("no such field. field=%s.%s.%s", db->name(), table->name(), relation_attr.attribute_name.c_str());
        return RC::SCHEMA_FIELD_MISSING;
      }

      query_fields.push_back(Field(table, field_meta));
      // 看有没有聚合查询
      if (relation_attr.func != NONE){
        for (auto iter = query_fields.begin() + length; iter != query_fields.end(); iter++)
          iter->set_func(relation_attr.func);
      }
    }
  }

  // 仿照query fields也录入一下group by的列名
  int group_by_begin;
  const std::vector<std::string> &group_by_columns = select_sql.group_by_columns;
  if (group_by_columns.size() != 0)
    group_by_begin = query_fields.size();
  else
    group_by_begin = -1;
  // 没有group by *.column
  for (std::string column : group_by_columns){
    size_t pos;
    if ((pos = column.find(".")) != column.npos){
      std::string table_name = column.substr(0, pos).c_str();
      std::string field_name = column.substr(pos + 1).c_str();
      auto iter = table_map.find(table_name);
      if (iter == table_map.end()) {
        LOG_WARN("no such table in from list: %s", table_name);
        return RC::SCHEMA_FIELD_MISSING;
      }

      Table *table = iter->second;
      const FieldMeta *field_meta = table->table_meta().field(field_name.c_str());
      if (nullptr == field_meta) {
        LOG_WARN("no such field. field=%s.%s.%s", db->name(), table->name(), field_name);
        return RC::SCHEMA_FIELD_MISSING;
      }

      query_fields.push_back(Field(table, field_meta));
    }
    else{
      if (tables.size() != 1) {
        LOG_WARN("invalid. I do not know the attr's table. attr=%s", column.c_str());
        return RC::SCHEMA_FIELD_MISSING;
      }

      Table *table = tables[0];
      const FieldMeta *field_meta = table->table_meta().field(column.c_str());
      if (nullptr == field_meta) {
        LOG_WARN("no such field. field=%s.%s.%s", db->name(), table->name(), column.c_str());
        return RC::SCHEMA_FIELD_MISSING;
      }

      query_fields.push_back(Field(table, field_meta));
    }
  }

  LOG_INFO("got %d tables in from stmt and %d fields in query stmt", tables.size(), query_fields.size());

  Table *default_table = nullptr;
  if (tables.size() == 1) {
    default_table = tables[0];
  }

  // create filter statement in `where` statement
  FilterStmt *filter_stmt = nullptr;
  RC rc = FilterStmt::create(db,
      default_table,
      &table_map,
      select_sql.conditions.data(),
      static_cast<int>(select_sql.conditions.size()),
      filter_stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("cannot construct filter stmt");
    return rc;
  }

  //检查简单字段和聚合字段是否在没有group by的情况下同时存在
  bool agg_exist = false;
  if (group_by_begin == -1){
    for (int i = 0; i < query_fields.size(); i ++){
      if (query_fields[i].func() != AggregationFunc::NONE)
        agg_exist = true;
      else{
        if (agg_exist)
          return RC::INVALID_ARGUMENT;
      }
    }
  }

  // everything alright
  SelectStmt *select_stmt = new SelectStmt();
  // TODO add expression copy
  select_stmt->tables_.swap(tables);
  select_stmt->query_fields_.swap(query_fields);
  select_stmt->filter_stmt_ = filter_stmt;
  select_stmt->group_by_begin_ = group_by_begin;
  stmt = select_stmt;
  return RC::SUCCESS;
}
