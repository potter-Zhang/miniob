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
 * @brief Group 表示group by运算
 * @ingroup LogicalOperator
 * @details 在project和group_by之上。
 */
class GroupLogicalOperator : public LogicalOperator 
{
public:
  GroupLogicalOperator(const std::vector<Field> &fields);
  GroupLogicalOperator(const std::vector<Field> &fields, std::unique_ptr<Expression> expression, int group_by_begin, int attr_having_begin);
  virtual ~GroupLogicalOperator() = default;

  LogicalOperatorType type() const override
  {
    return LogicalOperatorType::GROUP;
  }

  std::vector<std::unique_ptr<Expression>> &expressions()
  {
    return expressions_;
  }
  const std::vector<std::unique_ptr<Expression>> &expressions() const
  {
    return expressions_;
  }
  const int group_by_begin() const
  {
    return group_by_begin_;
  }
  const int attr_having_begin() const
  {
    return attr_having_begin_;
  }
  const std::vector<Field> &fields() const
  {
    return fields_;
  }

private:
  int group_by_begin_ = -1;
  int attr_having_begin_ = -1;
  std::vector<Field> fields_;
};
