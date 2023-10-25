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
// Created by WangYunlai on 2022/6/27.
//

#include "common/log/log.h"
#include "sql/operator/predicate_physical_operator.h"
#include "sql/operator/project_physical_operator.h"
#include "storage/record/record.h"
#include "sql/stmt/filter_stmt.h"
#include "storage/field/field.h"

PredicatePhysicalOperator::PredicatePhysicalOperator(std::unique_ptr<Expression> expr) : expression_(std::move(expr))
{
  ASSERT(expression_->value_type() == BOOLEANS, "predicate's expression should be BOOLEAN type");
}

RC PredicatePhysicalOperator::open(Trx *trx)
{
  RC rc = RC::SUCCESS;
  if (children_.size() != 1) {
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

  return children_[0]->open(trx);
}

/*
while (RC::SUCCESS == (rc = sql_result->next_tuple(tuple))) {
    assert(tuple != nullptr);

    int cell_num = tuple->cell_num();
    for (int i = 0; i < cell_num; i++) {
      if (i != 0) {
        const char *delim = " | ";
        rc = writer_->writen(delim, strlen(delim));
        if (OB_FAIL(rc)) {
          LOG_WARN("failed to send data to client. err=%s", strerror(errno));
          sql_result->close();
          return rc;
        }
      }

      Value value;
      rc = tuple->cell_at(i, value);
      if (rc != RC::SUCCESS) {
        sql_result->close();
        return rc;
      }

*/

RC PredicatePhysicalOperator::next()
{
  RC rc = RC::SUCCESS;
  PhysicalOperator *oper = children_.front().get();
  
  
  while (RC::SUCCESS == (rc = oper->next())) {
    Tuple *tuple = oper->current_tuple();
    if (nullptr == tuple) {
      rc = RC::INTERNAL;
      LOG_WARN("failed to get tuple from operator");
      break;
    }

    Value value;
    if (children_.size() != 1) {
      rc = expression_->get_value(*tuple, value, values_);
    }
    else {
      rc = expression_->get_value(*tuple, value);
    }
    
    if (rc != RC::SUCCESS) {
      return rc;
    }

    if (value.get_boolean()) {
      return rc;
    }
  }
  return rc;
}

RC PredicatePhysicalOperator::close()
{
  children_[0]->close();
  return RC::SUCCESS;
}

Tuple *PredicatePhysicalOperator::current_tuple()
{
  return children_[0]->current_tuple();
}
