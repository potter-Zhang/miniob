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
// Created by WangYunlai on 2022/12/08.
//

#pragma once

#include <vector>
#include <memory>

#include "sql/operator/logical_operator.h"
#include "sql/expr/expression.h"
#include "storage/field/field.h"

/**
 * @brief aggregation 表示聚合运算
 * @ingroup LogicalOperator
 * @details 在project和group_by之上。
 */
class AggregationLogicalOperator : public LogicalOperator 
{
public:
  AggregationLogicalOperator(const std::vector<AggregationFunc> &funcs, const std::vector<Field> &fields);
  AggregationLogicalOperator(const std::vector<AggregationFunc> &funcs, const std::vector<Field> &fields, bool group_by_exist);
  virtual ~AggregationLogicalOperator() = default;

  LogicalOperatorType type() const override
  {
    return LogicalOperatorType::AGGREGATION;
  }

  std::vector<std::shared_ptr<Expression>> &expressions()
  {
    return expressions_;
  }
  const std::vector<std::shared_ptr<Expression>> &expressions() const
  {
    return expressions_;
  }
  const std::vector<AggregationFunc> &funcs() const
  {
    return funcs_;
  }
  const std::vector<Field> fields() const{
    return fields_;
  }
  const bool group_by_exist() const
  {
    return group_by_exist_;
  }
  const void set_group_by_exist(bool group_by_exist)
  {
    group_by_exist_ = group_by_exist;
  }

private:
  std::vector<AggregationFunc> funcs_;
  std::vector<Field> fields_;

  bool group_by_exist_ = false;
};
