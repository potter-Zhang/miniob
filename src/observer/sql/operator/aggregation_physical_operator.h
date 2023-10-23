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

/**
 * @brief 聚合物理算子
 * @ingroup PhysicalOperator
 */
class AggregationPhysicalOperator : public PhysicalOperator
{
public:
  AggregationPhysicalOperator();
  
  AggregationPhysicalOperator(bool group_by_exist);

  virtual ~AggregationPhysicalOperator();

  PhysicalOperatorType type() const override
  {
    return PhysicalOperatorType::AGGREGATION;
  }

  RC open(Trx *trx) override;
  RC next() override;
  RC close() override;

  int cell_num() const
  {
    return tuple_->cell_num();
  }

  Tuple *current_tuple() override;
  const std::vector<AggregationFunc> funcs() const{
    return funcs_;
  }
  const std::vector<Field> fields() const{
    return fields_;
  }

  void add_func(AggregationFunc func);
  void add_field(const Field &field);

  const Tuple* tuple() const{
    return tuple_;
  }

private:
  RC do_aggregation();
  RC do_aggregation_(std::vector<Value> &values, std::vector<ValueListTuple *> &tuples, int cell_num);

private:
  ValueListTuple* tuple_;
  std::vector<Field> fields_;
  std::vector<AggregationFunc> funcs_;
  bool group_by_exist_ = false;
  std::vector<ValueListTuple *> tuples_;
  std::vector<ValueListTuple *>::iterator iter_;
};
