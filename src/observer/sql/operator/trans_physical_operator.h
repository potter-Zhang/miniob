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
// Created by WangYunlai on 2022/07/01.
//

#pragma once

#include "sql/operator/physical_operator.h"
#include "sql/expr/tuple.h"

class TransformPhysicalOperator : public PhysicalOperator
{
public:
  TransformPhysicalOperator(std::vector<std::unique_ptr<Expression>> &&expressions)
    : expressions_(std::move(expressions))
  {}

  virtual ~TransformPhysicalOperator() = default;

  PhysicalOperatorType type() const override
  {
    return PhysicalOperatorType::TRANSFORM;
  }

  std::string name() const override
  {
    return "TRANSFORM";
  }
  std::string param() const override
  {
    return "";
  }

  RC open(Trx *trx) override 
  { 
    if (children_.empty()) {
        return RC::SUCCESS;
    }
    return children_[0]->open(trx);
  }

  RC next() override
  {
    RC rc = RC::SUCCESS;
    PhysicalOperator *oper = children_.front().get();
    
    while (RC::SUCCESS == (rc = oper->next()))
    {
        Tuple *tuple = oper->current_tuple();
        if (nullptr == tuple) {
            rc = RC::INTERNAL;
            break;
        }

        
        std::vector<Value> values;
        for (int i = 0; i < expressions_.size(); i++) {
          if (expressions_[i] != nullptr) {
            Value value;
            expressions_[i]->get_value(*tuple, value);
            values.emplace_back(value);
          }
          else {
            Value value;
            int cell_num = tuple->cell_num();
            for (int i = 0; i < cell_num; i++) {
                tuple->cell_at(i, value);
                values.emplace_back(value);
            }
            break;
          }
        }
       
        tuple_.set_cells(values);
        return RC::SUCCESS;

    }
    
    return rc;
  }

  RC close() override { return children_[0]->close(); }

  int cell_num() const
  {
    return tuple_.cell_num();
  }

  Tuple *current_tuple() override
  {
    return &tuple_;
    //return children_[0]->current_tuple();
  }

  std::vector<std::unique_ptr<Expression>> &expressions()
  {
    return expressions_;
  }

private:
  std::vector<std::unique_ptr<Expression>> expressions_;
  ValueListTuple tuple_;
  Trx *trx_;
  
};
