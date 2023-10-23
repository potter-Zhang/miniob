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
class GroupPhysicalOperator : public PhysicalOperator
{
typedef std::unordered_map<std::vector<Value>, std::vector<ValueListTuple*>, vector_value_hash_name> groups_map_type;
public:
  GroupPhysicalOperator()
  {}

  GroupPhysicalOperator(int group_by_begin);

  virtual ~GroupPhysicalOperator();

  PhysicalOperatorType type() const override
  {
    return PhysicalOperatorType::GROUP;
  }

  RC open(Trx *trx) override;
  RC next() override;
  RC close() override;

  int cell_num() const
  {
    return tuple_->cell_num();
  }

  Tuple *current_tuple() override;

  void add_field(const Field &field);

  std::vector<Field> fields() const{
    return fields_;
  }
  const groups_map_type groups() const{
    return groups_;
  }
  const Tuple* tuple() const{
    return tuple_;
  }
  const int group_by_begin() const{
    return group_by_begin_;
  }

private:
  RC make_groups();

private:  
  Tuple* tuple_;
  groups_map_type groups_;
  int group_by_begin_ = -1;
  std::vector<Field> fields_;
  groups_map_type::iterator iter_;
};
