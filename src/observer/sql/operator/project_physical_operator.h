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
 * @brief 选择/投影物理算子
 * @ingroup PhysicalOperator
 */
class ProjectPhysicalOperator : public PhysicalOperator
{
public:
  ProjectPhysicalOperator()
  {}

  //ProjectPhysicalOperator(int group_by_begin);

  virtual ~ProjectPhysicalOperator() = default;

  void add_expressions(std::vector<std::unique_ptr<Expression>> &&expressions)
  {
    
  }
  void add_projection(const Table *table, const FieldMeta *field, const AggregationFunc func = NONE);

  PhysicalOperatorType type() const override
  {
    return PhysicalOperatorType::PROJECT;
  }

  RC open(Trx *trx) override;
  RC next() override;
  RC close() override;

  int cell_num() const
  {
    return tuple_.cell_num();
  }

  Tuple *current_tuple() override;
  std::vector<AggregationFunc> funcs() const{
    return funcs_;
  }
  const ProjectTuple& tuple() const{
    return tuple_;
  }
  /*const bool is_aggregation() const{
    return is_aggregation_;
  }*/
  /*const int group_by_begin() const{
    return group_by_begin_;
  }*/

private:
  ProjectTuple tuple_;
  std::vector<AggregationFunc> funcs_;
  //bool is_aggregation_ = false;
  //int group_by_begin_ = -1;
};
