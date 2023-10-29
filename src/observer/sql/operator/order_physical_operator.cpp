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
#include "sql/operator/order_physical_operator.h"
#include "storage/record/record.h"
#include "storage/table/table.h"

OrderPhysicalOperator::OrderPhysicalOperator(std::vector<bool> is_asc, int order_by_begin) : is_asc_(is_asc), order_by_begin_(order_by_begin){}

OrderPhysicalOperator::~OrderPhysicalOperator() 
{
  for (ValueListTuple* tuple : tuples_){
    delete tuple;
  }
  tuples_.clear();
}

RC OrderPhysicalOperator::open(Trx *trx) 
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

  sort();
  if (tuples_.size() > 0)
    iter_ = tuples_.begin();

  return RC::SUCCESS; 
}

RC OrderPhysicalOperator::next() 
{
  if (tuples_.size() == 0 || iter_ == tuples_.end())
    return RC::RECORD_EOF;
  tuple_ = *iter_;
  iter_ ++;
  return RC::SUCCESS;
}

RC OrderPhysicalOperator::close() 
{ 
  if (!children_.empty()) {
    children_[0]->close();
  }
  return RC::SUCCESS;
}

Tuple *OrderPhysicalOperator::current_tuple() 
{  
  return tuple_;
}

RC OrderPhysicalOperator::sort() 
{
  std::vector<ValueListTuple*> values;
  std::vector<ValueListTuple *> tuples;
  int cell_num = -1;

  while (children_[0]->next() != RC::RECORD_EOF){
    Tuple* tuple = children_[0]->current_tuple();
    ValueListTuple* tuple_copy = new ValueListTuple();
    std::vector<Value> values;
    int cell_num = tuple->cell_num();
    for (int i = 0; i < cell_num; i ++){
      Value value;
      RC rc = tuple->cell_at(i, value);
      if (rc != RC::SUCCESS) {
          return rc;    
      }
      values.push_back(value);
    }
    tuple_copy->set_cells_copy(values);
    tuples.push_back(tuple_copy);
  }
  if (tuples.size() == 0)
    return RC::SUCCESS;
  else
   cell_num = tuples[0]->cell_num();

  //for(int i = order_by_begin_; i < cell_num; i++)
    sort_(values, tuples);//, i - order_by_begin_);

  for(ValueListTuple* tuple : values) {
    std::vector<Value> tuple_values = tuple->cells();
    tuple_values.erase(tuple_values.begin() + order_by_begin_, tuple_values.end());
    ValueListTuple* value_tuple = new ValueListTuple();
    value_tuple->set_cells(tuple_values);
    tuples_.push_back(value_tuple);
  }

  // 最后再delete，否则values里面的东西就不能用了
  for(Tuple* tuple : tuples)
    delete static_cast<ValueListTuple *>(tuple);
}

RC OrderPhysicalOperator::sort_(std::vector<ValueListTuple*> &values, std::vector<ValueListTuple *> &tuples){//, int is_asc_current){
  values.clear();
  if (tuples.size() == 0)
    return RC::SUCCESS;
  int cell_num = tuples[0]->cell_num();
  std::map<std::vector<Value>, ValueListTuple*> order_filter;

  for (ValueListTuple* tuple : tuples) {
    std::vector<Value> values;
    values = tuple->cells();;
    values.erase(values.begin(), values.begin() + order_by_begin_);
    for (int i = 0; i < values.size(); i ++)
      values[i].set_asc(is_asc_[i]);
    order_filter.insert(std::pair<std::vector<Value>, ValueListTuple*>(values, tuple));
  }

  //if (is_asc_[is_asc_current])
    for(auto iter = order_filter.begin(); iter != order_filter.end(); iter ++)
      values.push_back(iter->second);
  /*else
    for(auto iter = order_filter.rbegin(); iter != order_filter.rend(); iter ++)
      values.push_back(iter->second);*/
  return RC::SUCCESS;
}