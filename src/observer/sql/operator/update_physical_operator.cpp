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

  if (children_.size() != 1) {
    std::unique_ptr<PhysicalOperator> &select_child = children_[1];
    rc = select_child->open(trx);
      if (rc != RC::SUCCESS) {
      LOG_WARN("failed to open child operator: %s", strrc(rc));
      return rc;
    }
    int num = 0;
    while (RC::SUCCESS == (rc = select_child->next())) {
      if (num++ >= 1) {
        return RC::INTERNAL;
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
      attr_value_pair_[0].value = value;
      
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
*/

RC UpdatePhysicalOperator::next()
{
  RC rc = RC::SUCCESS;
  if (children_.empty()) {
    return RC::RECORD_EOF;
  }

  PhysicalOperator *child = children_[0].get();
  while (RC::SUCCESS == (rc = child->next())) {
    Tuple *tuple = child->current_tuple();
    if (nullptr == tuple) {
      LOG_WARN("failed to get current record: %s", strrc(rc));
      return rc;
    }

    RowTuple *row_tuple = static_cast<RowTuple *>(tuple);
    Record &record = row_tuple->record();
    for (int i = 0; i < attr_value_pair_.size(); i++) {
       rc = trx_->modify_record(table_, record, attr_value_pair_[i].attribute_name.c_str(), attr_value_pair_[i].value);
      if (rc != RC::SUCCESS) {
        return rc;
      }
    }
  }
   

  return RC::RECORD_EOF;
}

RC UpdatePhysicalOperator::close()
{
  if (!children_.empty()) {
    children_[0]->close();
  }
  return RC::SUCCESS;
}
