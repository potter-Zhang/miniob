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
class OrderPhysicalOperator : public PhysicalOperator
{
public:
  OrderPhysicalOperator(std::vector<bool> is_asc, int order_by_begin);

  virtual ~OrderPhysicalOperator();

  PhysicalOperatorType type() const override
  {
    return PhysicalOperatorType::ORDER;
  }

  RC open(Trx *trx) override;
  RC next() override;
  RC close() override;

  int cell_num() const
  {
    return tuple_->cell_num();
  }

  Tuple *current_tuple() override;

  const Tuple* tuple() const{
    return tuple_;
  }

private:
  RC sort();
  RC sort_(std::vector<ValueListTuple*> &values, std::vector<ValueListTuple *> &tuples, int is_asc_current);

private:
  ValueListTuple* tuple_;
  std::vector<bool> is_asc_;
  int order_by_begin_ = -1;
  std::vector<ValueListTuple *> tuples_;
  std::vector<ValueListTuple *>::iterator iter_;
};
