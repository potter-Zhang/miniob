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
// Created by WangYunlai on 2021/6/9.
//

#include "sql/operator/update_physical_operator.h"
#include "sql/stmt/update_stmt.h"
#include "storage/table/table.h"
#include "storage/trx/trx.h"

using namespace std;

UpdatePhysicalOperator::UpdatePhysicalOperator(Table *table, std::vector<AttrValuePair> &attr_value_pair)
    : table_(table), attr_value_pair_(attr_value_pair)
{}

RC UpdatePhysicalOperator::open(Trx *trx)
{
  if (children_.empty()) {
    return RC::SUCCESS;
  }

  std::unique_ptr<PhysicalOperator> &child = children_[0];
  RC rc = child->open(trx);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open child operator: %s", strrc(rc));
    return rc;
  }
  int idx = 1;
  for (int i = 0; i < attr_value_pair_.size(); i++) {
    if (attr_value_pair_[i].ptr == nullptr) {
      continue;
    }
    std::unique_ptr<PhysicalOperator> &select_child = children_[idx++];
    rc = select_child->open(trx);
      if (rc != RC::SUCCESS) {
      LOG_WARN("failed to open child operator: %s", strrc(rc));
      return rc;
    }
    int num = 0;
    const FieldMeta* fieldmeta = table_->table_meta().field(attr_value_pair_[i].attribute_name.c_str());
    while (RC::SUCCESS == (rc = select_child->next())) {
      if (num++ >= 1) {
        delay_rc = RC::INTERNAL;
        return RC::SUCCESS;
      }
      Tuple *tuple = select_child->current_tuple();
      if (nullptr == tuple) {
        return RC::INTERNAL;
      }
      int cell_num = tuple->cell_num();
      if (cell_num != 1) {
        return RC::INTERNAL;
      }

      Value value;
      rc = tuple->cell_at(0, value);
      if (rc != RC::SUCCESS) {
        return rc;
      }
      value.convert_to(fieldmeta->type());
      attr_value_pair_[i].value = value;
      
    }
    if (num == 0) {
      if (fieldmeta->nullable()) {
        Value value;
        value.set_nullable(true);
        value.set_is_null(true);
        value.set_length(fieldmeta->len());
        value.get_data();
        attr_value_pair_[i].value = value;
      }
      else {
        delay_rc = RC::INTERNAL;
        return RC::SUCCESS;
      }
    }


  }

  trx_ = trx;

  return RC::SUCCESS;
}
/**
 * if (children_.size() != 1) {
    LOG_WARN("predicate operator must has one child");
    children_[1]->open(trx);
    if (children_.size() != 1) {
    PhysicalOperator *sub_oper = children_[1].get();
    while (RC::SUCCESS == (rc = sub_oper->next())) {
      Tuple *tuple = sub_oper->current_tuple();
      if (nullptr == tuple) {
        rc = RC::INTERNAL;
        return rc;
      }

      int cell_num = tuple->cell_num();
      if (cell_num != 1) {
        return RC::INTERNAL;
      }

      Value value;
      rc = tuple->cell_at(0, value);
      if (rc != RC::SUCCESS) {
        return rc;
      }
      values_.push_back(value);
      //printf("%s\n", value.to_string());
     }
  }
  }

  for (int i = 0; i < attr_value_pair_.size(); i++) {
       rc = trx_->modify_record(table_, record, attr_value_pair_[i].attribute_name.c_str(), attr_value_pair_[i].value);
      if (rc != RC::SUCCESS) {
        return rc;
      }
    }
*/

RC UpdatePhysicalOperator::next()
{
  RC rc = RC::SUCCESS;
  if (children_.empty()) {
    return RC::RECORD_EOF;
  }

  PhysicalOperator *child = children_[0].get();
  
  std::vector<Record> records;
  std::vector<Record> old_records;
  while (RC::SUCCESS == (rc = child->next())) {
    if (delay_rc != RC::SUCCESS) {
      return RC::INTERNAL;
    }
    Tuple *tuple = child->current_tuple();
    if (nullptr == tuple) {
      LOG_WARN("failed to get current record: %s", strrc(rc));
      return rc;
    }

    RowTuple *row_tuple = static_cast<RowTuple *>(tuple);
    Record &record = row_tuple->record();

    records.emplace_back(record);
    trx_->delete_index(table_, record);
    
  }
  if (records.empty()) {
    return RC::RECORD_EOF;
  }

  for (Record &record : records) {
    Record cpy;
    rc = modify(record, cpy);
     old_records.emplace_back(cpy);
    if (OB_FAIL(rc)) {
      goto ROLLBACK;
    }
   
  }
  int i;
  for (i = 0; i < old_records.size(); i++) {
    rc = trx_->insert_index(table_, records[i]);
    if (OB_FAIL(rc)) {
      break;
    }
  }

  if (OB_FAIL(rc)) {
    for (i = i - 1; i >= 0; i--) {
      if (trx_->delete_index(table_, records[i]) != RC::SUCCESS) {
        return RC::INTERNAL;
      }
    }
  }

  if (rc == RC::SUCCESS) {
    return RC::RECORD_EOF;
  }
  
  ROLLBACK: 
  for (int i = 0; i < old_records.size(); i++) {
    memcpy(records[i].data(), old_records[i].data(), table_->table_meta().record_size());
  }
    
  for (i = 0; i < old_records.size(); i++) {
    trx_->insert_index(table_, records[i]);
  }
  return RC::INTERNAL;
}

/**
 * const FieldMeta *field = table_meta_.field(field_name);
  if (field->nullable())
  {
    new_value.set_nullable(true);
    new_value.get_data();
  }
  else{
    if (new_value.nullable() && new_value.is_null())
      return RC::INVALID_ARGUMENT;
  }
*/
RC UpdatePhysicalOperator::modify(Record &record, Record &new_record) 
{
  char *buf = (char *)malloc(table_->table_meta().record_size());
  memcpy(buf, record.data(), table_->table_meta().record_size());
  new_record.set_data_owner(buf, table_->table_meta().record_size());
  for (AttrValuePair &a_v_pair : attr_value_pair_) {
     const FieldMeta *field = table_->table_meta().field(a_v_pair.attribute_name.c_str());
     if (field->nullable())
      {
        a_v_pair.value.set_nullable(true);
        a_v_pair.value.get_data();
      }
      else{
        if (a_v_pair.value.nullable() && a_v_pair.value.is_null())
          return RC::INVALID_ARGUMENT;
      }
     size_t copy_len = field->len();
    if (field->type() == CHARS) {
      const size_t data_len = a_v_pair.value.length();
      if (copy_len > data_len) {
        copy_len = data_len + 1;
      }
    }
    memcpy(record.data() + field->offset(), a_v_pair.value.data(), copy_len);
  }
  return RC::SUCCESS;
}


RC UpdatePhysicalOperator::close()
{
  if (!children_.empty()) {
    children_[0]->close();
  }
  return RC::SUCCESS;
}
