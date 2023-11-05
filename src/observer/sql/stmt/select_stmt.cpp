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
#include <algorithm>

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

  SelectStmt *select_stmt = new SelectStmt();

  select_stmt->table_alias_ = select_sql.table_alias;
  //检查在同一层中是否有不同表用了相同的别名
  std::vector<std::string> check;
  for (auto iter = select_stmt->table_alias_.begin(); iter != select_stmt->table_alias_.end(); iter ++) {
    if (std::find(check.begin(), check.end(), iter->second) != check.end())
      return RC::INVALID_ARGUMENT;
    check.emplace_back(iter->second);
  }

  select_stmt->column_alias_ = select_sql.column_alias;

  // collect tables in `from` statement
  std::vector<Table *> tables;
  std::unordered_map<std::string, Table *> table_map;

  for (auto iter = select_stmt->table_alias_.begin(); iter != select_stmt->table_alias_.end(); iter ++) {
    Table* table = db->find_table(iter->second.c_str());
    table_map.insert(std::pair<std::string, Table *>(iter->first, table));
    tables.push_back(table);
  }  

  for (size_t i = 0; i < select_sql.relations.size(); i++) {
    const char *table_name = select_sql.relations[i].c_str();
    if (nullptr == table_name) {
      LOG_WARN("invalid argument. relation name is null. index=%d", i);
      return RC::INVALID_ARGUMENT;
    }

    if (table_map.find(table_name) != table_map.end())
      continue;
    Table *table = db->find_table(table_name);
    if (nullptr == table) {      
      LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
      return RC::SCHEMA_TABLE_NOT_EXIST;
    }
    else if (std::find(tables.begin(), tables.end(), table) != tables.end())
      return RC::INVALID_ARGUMENT;

    tables.push_back(table);
    table_map.insert(std::pair<std::string, Table *>(table_name, table));
    select_stmt->table_alias_.insert(std::pair<std::string, std::string>(table_name, table_name));
  }

  std::unordered_map<std::string, std::string> table_alias = select_stmt->table_alias_;

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
      filter_stmt, table_alias);
  if (rc != RC::SUCCESS) {
    LOG_WARN("cannot construct filter stmt");
    return rc;
  }


  // 把having子句插入fitler_stmt中
  int having_begin;
  int having_num = static_cast<int>(select_sql.having_conditions.size());
  if (having_num == 0)
    having_begin = -1;
  else
    having_begin = static_cast<int>(select_sql.conditions.size());

  rc = filter_stmt->add_filter_unit(db,
        default_table,
        &table_map,
        select_sql.having_conditions.data(),
        static_cast<int>(select_sql.having_conditions.size()));
  if(rc != RC::SUCCESS) {
    LOG_WARN("cannot add having's filter conditions to filter_stmt");
    return rc;
  }

  // 把having中的字段放到query_fields里
  int attr_having_begin = having_num == 0 ? -1 : query_fields.size();
  for (ConditionSqlNode condition : select_sql.having_conditions){
    const char *table_name;
    const char *field_name;
    AggregationFunc func;
    if (condition.left_is_attr) {
      field_name = condition.left_attr.attribute_name.c_str(); 
      func = condition.left_attr.func;
    }
    else {
      assert(condition.right_is_attr);
      field_name = condition.right_attr.attribute_name.c_str();
    }

    const FieldMeta *field_meta;
    const TableMeta& table_meta = default_table->table_meta();
    if (0 == strcmp(field_name, "*"))
      field_meta = table_meta.field(table_meta.sys_field_num());
    else
      field_meta = table_meta.field(field_name);
    query_fields.push_back(Field(default_table, field_meta));
    query_fields.back().set_func(func);
  }

  LOG_INFO("got %d tables in from stmt and %d fields in query stmt", tables.size(), query_fields.size());

  //检查简单字段和聚合字段是否在没有group by的情况下同时存在
  bool agg_exist = false;
  int non_agg_num = 0;
  if (group_by_begin == -1){
    for (int i = 0; i < query_fields.size(); i ++){
      if (query_fields[i].func() != AggregationFunc::NONE) {
        if (non_agg_num > 0)
          return RC::INVALID_ARGUMENT;
        agg_exist = true;
      }
      else{
        non_agg_num ++;
        if (agg_exist)
          return RC::INVALID_ARGUMENT;
      }
    }
  }

  //插入order字段
  int order_num = select_sql.order_columns.size();
  int order_by_begin = order_num == 0 ? -1 : query_fields.size();
  std::vector<bool> is_asc;
  if (order_by_begin != -1) {
    for (int i = order_num - 1; i >= 0; i--) {
      const std::pair<RelAttrSqlNode, bool> &order_column = select_sql.order_columns[i];
      is_asc.push_back(order_column.second);
      const RelAttrSqlNode &relation_attr = order_column.first;
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
  }



  std::vector<std::unique_ptr<Expression>> expressions;
  for (auto iter = select_sql.attributes.rbegin(); iter != select_sql.attributes.rend(); iter ++) {
    if (iter->attribute_name == "*") {      
      if (iter->func == NONE) {
        std::vector<Field> fields;
        if (iter->relation_name != "")
          wildcard_fields(table_map[iter->relation_name] ,fields);
        else
          for (Table* table : tables)
            wildcard_fields(table, fields);
        for (Field field : fields)
          expressions.emplace_back(std::move(std::unique_ptr<Expression>(new FieldExpr(field))));
      }
      else {
        for (Table *table : tables){
          const TableMeta &table_meta = table->table_meta();
          const int field_num = table_meta.field_num();
          Field field(table, table_meta.field(table_meta.sys_field_num()));
          field.set_is_star(true);
          field.set_func(COUNTFUN);
          expressions.emplace_back(std::move(std::unique_ptr<Expression>(new FieldExpr(field))));
        }
      }
    }
    else {
      expressions.emplace_back(iter->expr);
    }
  }
  for (auto &expr : expressions) {
    if (expr == nullptr)
      continue;
    if (static_cast<FieldExpr *>(expr.get())->rel().attribute_name == "")
      continue;
    if (OB_FAIL(set_up_expression(db, default_table, &table_map, expr.get()))) {
      return RC::INTERNAL;
    }
  }

  // everything alright
  //SelectStmt *select_stmt = new SelectStmt();
  // TODO add expression copy
  select_stmt->tables_.swap(tables);
  select_stmt->query_fields_.swap(query_fields);
  select_stmt->is_asc_.swap(is_asc);
  select_stmt->filter_stmt_ = filter_stmt;
  select_stmt->group_by_begin_ = group_by_begin;
  select_stmt->having_begin_ = having_begin;
  select_stmt->attr_having_begin_ = attr_having_begin;
  select_stmt->order_by_begin_ = order_by_begin;
  select_stmt->expressions_ .swap(expressions);
  stmt = select_stmt;
  return RC::SUCCESS;
}

RC SelectStmt::create(Db *db, const SelectSqlNode &select_sql, Stmt *&stmt, std::unordered_map<std::string, std::string> &table_alias)
{
  if (nullptr == db) {
    LOG_WARN("invalid argument. db is null");
    return RC::INVALID_ARGUMENT;
  }

  SelectStmt *select_stmt = new SelectStmt();

  select_stmt->table_alias_ = select_sql.table_alias;
  std::vector<std::string> check;
  for (auto iter = select_stmt->table_alias_.begin(); iter != select_stmt->table_alias_.end(); iter ++) {
    if (std::find(check.begin(), check.end(), iter->second) != check.end())
      return RC::INVALID_ARGUMENT;
    check.emplace_back(iter->second);
  }
  //select_stmt->column_alias_ = select_sql.column_alias;

  // collect tables in `from` statement
  std::vector<Table *> tables;
  std::unordered_map<std::string, Table *> table_map;

  std::vector<Table *> raw_tables;

  Table *default_table = nullptr;
 
  for (auto iter = select_stmt->table_alias_.begin(); iter != select_stmt->table_alias_.end(); iter ++) {
    Table* table = db->find_table(iter->second.c_str());
    table_map.insert(std::pair<std::string, Table *>(iter->first, table));
    tables.push_back(table);
  }

  int num1= tables.size();
  if (num1 == 1)
    default_table = tables[0];

  std::unordered_map<std::string, std::string> table_alias_copy = select_stmt->table_alias_;

  std::unordered_map<std::string, Table *> table_map_local = table_map;

  for (auto iter = table_alias.begin(); iter != table_alias.end(); iter ++) {
    if (table_map.find(iter->first) != table_map.end())
      continue;
    Table* table = db->find_table(iter->second.c_str());
    table_map.insert(std::pair<std::string, Table *>(iter->first, table));
    tables.push_back(table);
    table_alias_copy.insert(std::pair<std::string, std::string>(iter->first, iter->second));
  }

  int num2 = tables.size();

  //int actual_table_num = 0;

  int need_help = 0;

  for (size_t i = 0; i < select_sql.relations.size(); i++) {
    const char *table_name = select_sql.relations[i].c_str();
    if (nullptr == table_name) {
      LOG_WARN("invalid argument. relation name is null. index=%d", i);
      return RC::INVALID_ARGUMENT;
    }

    auto iter = table_map.find(table_name);
    if (iter != table_map.end()) {
      need_help ++;
      raw_tables.push_back(iter->second);
      if (table_map_local.find(table_name) == table_map_local.end()) {
        Table *table = db->find_table(table_name);
        table_map_local.insert(std::pair<std::string, Table *>(table_name, table));
      }
      continue;
    }
    Table *table = db->find_table(table_name);
    if (nullptr == table) {      
      LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
      return RC::SCHEMA_TABLE_NOT_EXIST;
    }
    else if (std::find(tables.begin(), tables.end(), table) != tables.end())
      return RC::INVALID_ARGUMENT;

    tables.push_back(table);
    raw_tables.push_back(table);
    table_map.insert(std::pair<std::string, Table *>(table_name, table));
    table_map_local.insert(std::pair<std::string, Table *>(table_name, table));
    table_alias_copy.insert(std::pair<std::string, std::string>(table_name, table_name));
  }

  int num3 = tables.size();

  //assert(actual_table_num > 0);
  if (table_map_local.size() == 1)
    if (default_table == nullptr)
      default_table = table_map_local.begin()->second;
  else
    default_table = nullptr;

  std::vector<Table *> tables_to_swap;
  select_stmt->tables_.clear();
  for (auto iter = table_map_local.begin(); iter != table_map_local.end(); iter ++) {
    select_stmt->tables_.push_back(iter->second);
  }
  tables_to_swap = select_stmt->tables_;

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
        for (Table *table : tables_to_swap){
          const TableMeta &table_meta = table->table_meta();
          const int field_num = table_meta.field_num();
          query_fields.push_back(Field(table, table_meta.field(table_meta.sys_field_num())));
          query_fields.back().set_is_star(true);
        }
        for (auto iter = query_fields.begin() + length; iter != query_fields.end(); iter++)
          iter->set_func(relation_attr.func);
      }
      else{
        for (Table *table : tables_to_swap) {
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
          for (Table *table : tables_to_swap) {
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
      if (tables_to_swap.size() != 1) {
        LOG_WARN("invalid. I do not know the attr's table. attr=%s", relation_attr.attribute_name.c_str());
        return RC::SCHEMA_FIELD_MISSING;
      }

      Table *table = tables_to_swap[0];
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
      if (tables_to_swap.size() != 1) {
        LOG_WARN("invalid. I do not know the attr's table. attr=%s", column.c_str());
        return RC::SCHEMA_FIELD_MISSING;
      }

      Table *table = tables_to_swap[0];
      const FieldMeta *field_meta = table->table_meta().field(column.c_str());
      if (nullptr == field_meta) {
        LOG_WARN("no such field. field=%s.%s.%s", db->name(), table->name(), column.c_str());
        return RC::SCHEMA_FIELD_MISSING;
      }

      query_fields.push_back(Field(table, field_meta));
    }
  }    

  // create filter statement in `where` statement

  FilterStmt *filter_stmt = nullptr;
  RC rc = FilterStmt::create(db,
      default_table,
      &table_map_local,
      select_sql.conditions.data(),
      static_cast<int>(select_sql.conditions.size()),
      filter_stmt, table_alias_copy);
  if (rc != RC::SUCCESS) {
    LOG_WARN("cannot construct filter stmt");
    return rc;
  }

  // 把having子句插入fitler_stmt中
  int having_begin;
  int having_num = static_cast<int>(select_sql.having_conditions.size());
  if (having_num == 0)
    having_begin = -1;
  else
    having_begin = static_cast<int>(select_sql.conditions.size());

  rc = filter_stmt->add_filter_unit(db,
        default_table,
        &table_map_local,
        select_sql.having_conditions.data(),
        static_cast<int>(select_sql.having_conditions.size()));
  if(rc != RC::SUCCESS) {
    LOG_WARN("cannot add having's filter conditions to filter_stmt");
    return rc;
  }

  // 把having中的字段放到query_fields里
  int attr_having_begin = having_num == 0 ? -1 : query_fields.size();
  for (ConditionSqlNode condition : select_sql.having_conditions){
    const char *table_name;
    const char *field_name;
    AggregationFunc func;
    if (condition.left_is_attr) {
      field_name = condition.left_attr.attribute_name.c_str(); 
      func = condition.left_attr.func;
    }
    else {
      assert(condition.right_is_attr);
      field_name = condition.right_attr.attribute_name.c_str();
    }

    const FieldMeta *field_meta;
    const TableMeta& table_meta = default_table->table_meta();
    if (0 == strcmp(field_name, "*"))
      field_meta = table_meta.field(table_meta.sys_field_num());
    else
      field_meta = table_meta.field(field_name);
    query_fields.push_back(Field(default_table, field_meta));
    query_fields.back().set_func(func);
  }

  LOG_INFO("got %d tables in from stmt and %d fields in query stmt", tables_to_swap.size(), query_fields.size());

  //检查简单字段和聚合字段是否在没有group by的情况下同时存在
  bool agg_exist = false;
  int non_agg_num = 0;
  if (group_by_begin == -1){
    for (int i = 0; i < query_fields.size(); i ++) {
      if (query_fields[i].func() != AggregationFunc::NONE) {
        if (non_agg_num > 0)
          return RC::INVALID_ARGUMENT;
        agg_exist = true;
      }
      else{
        non_agg_num ++;
        if (agg_exist)
          return RC::INVALID_ARGUMENT;
      }
    }
  }

  //插入order字段
  int order_num = select_sql.order_columns.size();
  int order_by_begin = order_num == 0 ? -1 : query_fields.size();
  std::vector<bool> is_asc;
  if (order_by_begin != -1) {
    for (int i = order_num - 1; i >= 0; i--) {
      const std::pair<RelAttrSqlNode, bool> &order_column = select_sql.order_columns[i];
      is_asc.push_back(order_column.second);
      const RelAttrSqlNode &relation_attr = order_column.first;
      int length = query_fields.size();
      if (common::is_blank(relation_attr.relation_name.c_str()) &&
          0 == strcmp(relation_attr.attribute_name.c_str(), "*")) {
        // 看有没有聚合查询
        if (relation_attr.func != NONE){
          if (relation_attr.func != AggregationFunc::COUNTFUN){
            LOG_WARN("invalid aggregation query: AggregationFunc(%d) of *", relation_attr.func);
            return RC::SQL_SYNTAX;
          }
          for (Table *table : tables_to_swap){
            const TableMeta &table_meta = table->table_meta();
            const int field_num = table_meta.field_num();
            query_fields.push_back(Field(table, table_meta.field(table_meta.sys_field_num())));
            query_fields.back().set_is_star(true);
          }
          for (auto iter = query_fields.begin() + length; iter != query_fields.end(); iter++)
            iter->set_func(relation_attr.func);
        }
        else{
          for (Table *table : tables_to_swap) {
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
            for (Table *table : tables_to_swap) {
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
        if (tables_to_swap.size() != 1) {
          LOG_WARN("invalid. I do not know the attr's table. attr=%s", relation_attr.attribute_name.c_str());
          return RC::SCHEMA_FIELD_MISSING;
        }

        Table *table = tables_to_swap[0];
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
  }

  std::vector<std::unique_ptr<Expression>> expressions;
  for (auto iter = select_sql.attributes.rbegin(); iter != select_sql.attributes.rend(); iter ++) {
    if (iter->attribute_name == "*") {      
      if (iter->func == NONE) {
        std::vector<Field> fields;
        if (iter->relation_name != "")
          wildcard_fields(table_map[iter->relation_name] ,fields);
        else
          for (Table* table : tables_to_swap)//raw_tables)
            wildcard_fields(table, fields);
        for (Field field : fields)
          expressions.emplace_back(std::move(std::unique_ptr<Expression>(new FieldExpr(field))));
      }
      else {
        for (Table *table : tables_to_swap){
          const TableMeta &table_meta = table->table_meta();
          const int field_num = table_meta.field_num();
          Field field(table, table_meta.field(table_meta.sys_field_num()));
          field.set_is_star(true);
          field.set_func(COUNTFUN);
          expressions.emplace_back(std::move(std::unique_ptr<Expression>(new FieldExpr(field))));
        }
      }
    }
    else {
      expressions.emplace_back(iter->expr);
    }
  }
  for (auto &expr : expressions) {
    if (expr == nullptr)
      continue;
    if (static_cast<FieldExpr *>(expr.get())->rel().attribute_name == "")
      continue;
    if (OB_FAIL(set_up_expression(db, default_table, &table_map, expr.get()))) {
      return RC::INTERNAL;
    }
  }

  // everything alright 
  

  // TODO add expression copy
  //select_stmt->tables_.swap(tables);
  select_stmt->query_fields_.swap(query_fields);
  select_stmt->is_asc_.swap(is_asc);
  select_stmt->filter_stmt_ = filter_stmt;
  select_stmt->group_by_begin_ = group_by_begin;
  select_stmt->having_begin_ = having_begin;
  select_stmt->attr_having_begin_ = attr_having_begin;
  select_stmt->order_by_begin_ = order_by_begin;
  select_stmt->expressions_ .swap(expressions);
  stmt = select_stmt;
  return RC::SUCCESS;
}

RC SelectStmt::set_up_expression(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables, Expression *expr)
  {
     if (expr->type() == ExprType::FIELD) {
      RC rc;
      FieldExpr *field_expr = static_cast<FieldExpr *>(expr);
      Table *table = nullptr;
      const FieldMeta *field = nullptr;
      rc = field_expr->get_table_and_field(db, default_table, tables);
      if (rc != RC::SUCCESS) {
        
        return rc;
      }
      //field_expr->set_table_field(table, field);
      return rc;
    }
    if (expr->type() == ExprType::ARITHMETIC) {
      ArithmeticExpr *comp_expr = static_cast<ArithmeticExpr *>(expr);
      if (OB_FAIL(set_up_expression(db, default_table, tables, comp_expr->left().get()))) {
        return RC::INTERNAL;
      }
      if (comp_expr->arithmetic_type() != ArithmeticExpr::Type::NEGATIVE)
        if (OB_FAIL(set_up_expression(db, default_table, tables, comp_expr->right().get()))) {
          return RC::INTERNAL;
        }
      return RC::SUCCESS;
    }

    if (expr->type() == ExprType::FUNCTION) {
      FunctionExpr *func_expr = static_cast<FunctionExpr *>(expr);
      if (OB_FAIL(set_up_expression(db, default_table, tables, func_expr->expr()))) {
        return RC::INTERNAL;
      }
      return RC::SUCCESS;
    }
    
  }
