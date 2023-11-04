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

#include "common/log/log.h"
#include "sql/operator/group_physical_operator.h"
#include "storage/record/record.h"
#include "storage/table/table.h"
#include "group_physical_operator.h"

GroupPhysicalOperator::GroupPhysicalOperator(std::unique_ptr<Expression> expr, int group_by_begin, int attr_having_begin): 
  expression_(std::move(expr)), group_by_begin_(group_by_begin), attr_having_begin_(attr_having_begin) {}
GroupPhysicalOperator::~GroupPhysicalOperator() 
{
  /*for (Field field : fields_)
    delete &field;*/
  for (auto iter : groups_){
    for(ValueListTuple* tuple : iter.second)
      delete tuple;
  }
  if (tuple_ != nullptr)
    delete tuple_;
}
RC GroupPhysicalOperator::open(Trx *trx)
{
  if (children_.empty()) {
    return RC::SUCCESS;
  }

  PhysicalOperator *child = children_[0].get();
  RC rc = child->open(trx);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open child operator: %s", strrc(rc));
    return rc;
  }

  make_groups();
  if (groups_.size() > 0)
    iter_ = groups_.begin();

  tuple_ = new ValueListTuple();
  for (Field &field : fields_)
    tuple_->add_cell_spec(&field);
  return RC::SUCCESS;
}

RC GroupPhysicalOperator::make_groups()
{
  while (children_[0]->next() != RC::RECORD_EOF){
    std::vector<Value> values;
    std::vector<Value> group_values;
    Tuple* tuple = children_[0]->current_tuple();
    int cell_num = tuple->cell_num();
    for (int i = 0; i < group_by_begin_; i ++){
      Value value;
      RC rc = tuple->cell_at(i, value);
      if (rc != RC::SUCCESS) {
        return rc;    
      }
      values.push_back(value);
    }
    if (attr_having_begin_ > -1) {
      for (int i = attr_having_begin_; i < cell_num; i ++){
        Value value;
        RC rc = tuple->cell_at(i, value);
        if (rc != RC::SUCCESS) {
          return rc;    
        }
        values.push_back(value);
      }
    }

    for (int i = group_by_begin_; i < (attr_having_begin_ == -1 ? cell_num : attr_having_begin_); i ++) {
      Value value;
      RC rc = tuple->cell_at(i, value);
      if (rc != RC::SUCCESS) {
        return rc;    
      }
      group_values.push_back(value);
    }
    auto iter = groups_.find(group_values);
    if (iter == groups_.end()) {
      std::vector<ValueListTuple*> values_group;
      ValueListTuple* vlt = new ValueListTuple();
      vlt->set_cells(values);
      values_group.push_back(vlt);
      groups_.insert(std::pair<std::vector<Value>, std::vector<ValueListTuple*>>(group_values, values_group));
    }
    else{
      ValueListTuple* vlt = new ValueListTuple();
      vlt->set_cells(values);
      iter->second.push_back(vlt);
    }
  }
}

RC GroupPhysicalOperator::next() 
{ 
  if (groups_.size() == 0 || iter_ == groups_.end())
    return RC::RECORD_EOF;
  tuple_->set_cells(iter_->second[0]->cells());
  iter_ ++;
  return RC::SUCCESS;
}

RC GroupPhysicalOperator::close() 
{ 
  if (!children_.empty()) {
    children_[0]->close();
  }
  return RC::SUCCESS;
}

Tuple* GroupPhysicalOperator::current_tuple()
{
  return tuple_;
}

void GroupPhysicalOperator::add_field(const Field &field)
{
  //Field* field_copy = new Field(field);
  fields_.push_back(field);
}
