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
// Created by Wangyunlai on 2022/5/22.
//

#include "sql/stmt/update_stmt.h"
#include "sql/parser/parse_defs.h"
#include "sql/stmt/select_stmt.h"
#include "common/log/log.h"
#include "storage/db/db.h"
#include "storage/table/table.h"

UpdateStmt::UpdateStmt(Table *table, std::vector<AttrValuePair> &attr_value_pair_vec, FilterStmt *filter_stmt)
    : table_(table), attr_value_pairs(attr_value_pair_vec), filter_stmt_(filter_stmt)
{}

RC UpdateStmt::create(Db *db, UpdateSqlNode &update, Stmt *&stmt)
{
  const char *table_name = update.relation_name.c_str();
  if (nullptr == db || nullptr == table_name) {
    LOG_WARN("invalid argument. db=%p, table_name=%p, value_num=%d",
        db, table_name, 1);
    return RC::INVALID_ARGUMENT;
  }

  // check whether the table exists
  Table *table = db->find_table(table_name);
  if (nullptr == table) {
    LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  // check the fields number
  //Value &value = update.value;
  const TableMeta &table_meta = table->table_meta();
  const int field_num = table_meta.field_num() - table_meta.sys_field_num();

  // check fields type
  const int sys_field_num = table_meta.sys_field_num();
  for (int i = 0; i < update.attr_value_pairs.size(); i++) {
    
    const FieldMeta *field_meta = table_meta.field(update.attr_value_pairs[i].attribute_name.c_str());
    if (field_meta == nullptr) {
      return RC::INVALID_ARGUMENT;
    }
    if (update.attr_value_pairs[i].ptr != nullptr) {
      continue;
    }
    const AttrType field_type = field_meta->type();
    const AttrType value_type = update.attr_value_pairs[i].value.attr_type();
    if (field_type != value_type) {
      if (!update.attr_value_pairs[i].value.convert_to(field_type)) {
        return RC::SCHEMA_FIELD_TYPE_MISMATCH;
      }
      if (!field_meta->nullable()) {
        if (update.attr_value_pairs[i].value.nullable() && update.attr_value_pairs[i].value.is_null())
          return RC::SCHEMA_FIELD_TYPE_MISMATCH;
      }
    }
  }
  

  std::unordered_map<std::string, Table *> table_map;
  table_map.insert(std::pair<std::string, Table *>(std::string(table_name), table));

  FilterStmt *filter_stmt = nullptr;
  RC rc = FilterStmt::create(
      db, table, &table_map, update.conditions.data(), static_cast<int>(update.conditions.size()), filter_stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create filter statement. rc=%d:%s", rc, strrc(rc));
    return rc;
  }

  for (int i = 0; i < update.attr_value_pairs.size(); i++) {
    if (update.attr_value_pairs[i].ptr != nullptr) {
      SelectSqlNode selectSqlNode = *static_cast<SelectSqlNode *>(update.attr_value_pairs[i].ptr);
      Stmt *stmt = nullptr;
      rc = SelectStmt::create(db, selectSqlNode, stmt);
      if (OB_FAIL(rc)) {
        return rc;
      }
      update.attr_value_pairs[i].ptr = stmt;
    }
  }
  

  stmt = new UpdateStmt(table, update.attr_value_pairs, filter_stmt);
  
  return RC::SUCCESS;
  
}
