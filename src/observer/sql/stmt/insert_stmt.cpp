/* Copyright (c) 2021OceanBase and/or its affiliates. All rights reserved.
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

#include "sql/stmt/insert_stmt.h"
#include "common/log/log.h"
#include "storage/db/db.h"
#include "storage/table/table.h"
<<<<<<< HEAD
=======
#include "math.h"
>>>>>>> 6db5f5f0799d7ce0d38bcc99a331c86cb9777008

InsertStmt::InsertStmt(Table *table, const Value *values, int value_amount)
    : table_(table), values_(values), value_amount_(value_amount)
{}

RC InsertStmt::create(Db *db, InsertSqlNode &inserts, Stmt *&stmt)
{
  const char *table_name = inserts.relation_name.c_str();
  if (nullptr == db || nullptr == table_name || inserts.values.empty()) {
    LOG_WARN("invalid argument. db=%p, table_name=%p, value_num=%d",
        db, table_name, static_cast<int>(inserts.values.size()));
    return RC::INVALID_ARGUMENT;
  }

  // check whether the table exists
  Table *table = db->find_table(table_name);
  if (nullptr == table) {
    LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  // check the fields number
<<<<<<< HEAD
  std::vector<Value> &values = inserts.values;
  const int value_num = static_cast<int>(inserts.values.size());
  const TableMeta &table_meta = table->table_meta();
  const int field_num = table_meta.field_num() - table_meta.sys_field_num();
  if (field_num != value_num) {
=======
  Value *values = inserts.values.data();
  const int value_num = static_cast<int>(inserts.values.size());
  const TableMeta &table_meta = table->table_meta();
  const int field_num = table_meta.field_num() - table_meta.sys_field_num();
  if (value_num % field_num != 0) {
>>>>>>> 6db5f5f0799d7ce0d38bcc99a331c86cb9777008
    LOG_WARN("schema mismatch. value num=%d, field num in schema=%d", value_num, field_num);
    return RC::SCHEMA_FIELD_MISSING;
  }

<<<<<<< HEAD
  // check fields type
  const int sys_field_num = table_meta.sys_field_num();
  for (int i = 0; i < value_num; i++) {
    const FieldMeta *field_meta = table_meta.field(i + sys_field_num);
    const AttrType field_type = field_meta->type();
    const AttrType value_type = values[i].attr_type();
    if (field_type != value_type) {  
      // TODO try to convert the value type to field type
      if (field_type == AttrType::DATES && value_type == AttrType::CHARS) {
        values[i].convert_to(AttrType::DATES);
        continue;
      }
      LOG_WARN("field type mismatch. table=%s, field=%s, field type=%d, value_type=%d",
          table_name, field_meta->name(), field_type, value_type);
      return RC::SCHEMA_FIELD_TYPE_MISMATCH;
=======
  // check fields type 并且仅在fields type不匹配时检查nullable字段并决定是否进行转换
  const int sys_field_num = table_meta.sys_field_num();
  for (int i = 0; i < value_num; i++) {
    const FieldMeta *field_meta = table_meta.field(i % field_num + sys_field_num);
    const AttrType field_type = field_meta->type();
    const AttrType value_type = values[i].attr_type();    
    if (field_type != value_type) {  // TODO try to convert the value type to field type
      if (value_type == AttrType::NULLTYPE){
        if (field_meta->nullable()){
          values[i].set_nullable(true);
          values[i].set_type(field_type);
          values[i].set_length(field_meta->len());
          values[i].get_data();
        }
        else {
          LOG_WARN("field type mismatch. table=%s, field=%s, field type=%d, value_type=%d",
            table_name, field_meta->name(), field_type, value_type);
          return RC::SCHEMA_FIELD_TYPE_MISMATCH;
        }
      }
      else {
        LOG_WARN("field type mismatch. table=%s, field=%s, field type=%d, value_type=%d",
            table_name, field_meta->name(), field_type, value_type);
        return RC::SCHEMA_FIELD_TYPE_MISMATCH;
      }
    }
    else {
      values[i].set_nullable(field_meta->nullable());
      values[i].set_length(field_meta->len());
      values[i].get_data();
>>>>>>> 6db5f5f0799d7ce0d38bcc99a331c86cb9777008
    }
  }

  // everything alright
<<<<<<< HEAD
  stmt = new InsertStmt(table, values.data(), value_num);
=======
  stmt = new InsertStmt(table, values, value_num);
  ((InsertStmt*)stmt)->groups_ = round(value_num / field_num);
>>>>>>> 6db5f5f0799d7ce0d38bcc99a331c86cb9777008
  return RC::SUCCESS;
}
