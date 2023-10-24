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
#include "sql/operator/aggregation_physical_operator.h"
#include "storage/record/record.h"
#include "storage/table/table.h"
#include "group_physical_operator.h"
#include "aggregation_physical_operator.h"
#include "project_physical_operator.h"
#include "group_physical_operator.h"

AggregationPhysicalOperator::AggregationPhysicalOperator()
{
  if (children_[0]->type() == PhysicalOperatorType::GROUP)
    group_by_exist_ = true;
}
AggregationPhysicalOperator::AggregationPhysicalOperator(bool group_by_exist) : group_by_exist_(group_by_exist){}

AggregationPhysicalOperator::~AggregationPhysicalOperator() 
{
  for (ValueListTuple* tuple : tuples_){
    delete tuple;
  }
  tuples_.clear();
}

RC AggregationPhysicalOperator::open(Trx *trx) 
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

  do_aggregation();
  if (tuples_.size() > 0)
    iter_ = tuples_.begin();

  return RC::SUCCESS; 
}

RC AggregationPhysicalOperator::next() 
{
  if (tuples_.size() == 0 || iter_ == tuples_.end())
    return RC::RECORD_EOF;
  tuple_ = *iter_;
  iter_ ++;
  return RC::SUCCESS;
}

RC AggregationPhysicalOperator::close() 
{ 
  if (!children_.empty()) {
    children_[0]->close();
  }
  return RC::SUCCESS;
}

Tuple *AggregationPhysicalOperator::current_tuple() 
{  
  return tuple_;
}

void AggregationPhysicalOperator::add_func(AggregationFunc func) { funcs_.push_back(func); }

void AggregationPhysicalOperator::add_field(const Field &field) 
{
  //Field* field_copy = new Field(field);
  fields_.push_back(field);
}

RC AggregationPhysicalOperator::do_aggregation() 
{
  if (group_by_exist_)
  {
    assert(children_[0]->type() == PhysicalOperatorType::GROUP);
    GroupPhysicalOperator* child = static_cast<GroupPhysicalOperator *>(children_[0].get());
    int group_by_begin = child->group_by_begin();
    int attr_having_begin = child->attr_having_begin();
    Expression* exp;
    ConjunctionExpr *expression = nullptr;
    //std::vector<std::unique_ptr<Expression>> expressions;
    if (attr_having_begin != -1){
      exp = child->expression().get();
      // exp->type()既可能是CONJUNCTION，又可能是COMPARISON，因为logical_plan会被重写;
      if (exp->type() == ExprType::CONJUNCTION) {
        expression = static_cast<ConjunctionExpr *>(exp);
        //expressions = expression->children(); // 要直接用应用unique_ptr不存在拷贝构造函数
        assert(expression->children().size() > 0);
        assert(expression->children()[0]->type() == ExprType::COMPARISON);
      }
      else {
        assert(exp->type() == ExprType::COMPARISON);
      }
    }
    
    auto groups = child->groups();
    std::vector<Value> values;

    int avg_size = 0;
    for (auto iter = funcs_.begin(); iter != funcs_.end(); iter ++){
      if (*iter == AggregationFunc::AVGFUN)
        avg_size ++;
    }
    std::vector<int> num(avg_size, 0);

    for(auto group = groups.begin(); group != groups.end(); group ++) {
      do_aggregation_(values, group->second);//, funcs_.size());


      ValueListTuple* tuple = new ValueListTuple();

      bool flag = true; //判断having是否接受
      if (attr_having_begin != -1) {
        if (expression) {
          std::vector<std::unique_ptr<Expression>> &expressions = expression->children();
          for (int i = group_by_begin; i < values.size(); i ++){ //having_funcs起始位置在正常的values值的末尾+1处
            ComparisonExpr* comp = static_cast<ComparisonExpr *>(expressions[i - group_by_begin].get()); //保证从0开始
            CompOp op = comp->comp();
            if (comp->right()->type() == ExprType::FIELD){
              assert(comp->left()->type() == ExprType::VALUE);
              std::unique_ptr<Expression> left = std::unique_ptr<ValueExpr>(new ValueExpr(static_cast<ValueExpr*>(comp->left().get())->get_value()));
              std::unique_ptr<Expression> right = std::unique_ptr<ValueExpr>(new ValueExpr(values[i]));
              ComparisonExpr temp = ComparisonExpr(op, std::move(left), std::move(right));
              Value value;
              temp.get_value(*tuple, value);
              if (!value.get_boolean()){
                flag = false;
                break;
              }
            }
            else{
              assert(comp->left()->type() == ExprType::FIELD);
              assert(comp->right()->type() == ExprType::VALUE);
              std::unique_ptr<Expression> right = std::unique_ptr<ValueExpr>(new ValueExpr(static_cast<ValueExpr*>(comp->right().get())->get_value()));
              std::unique_ptr<Expression> left = std::unique_ptr<ValueExpr>(new ValueExpr(values[i]));
              ComparisonExpr temp = ComparisonExpr(op, std::move(left), std::move(right));
              Value value;
              temp.get_value(*tuple, value);
              if (!value.get_boolean()){
                flag = false;
                break;
              }
            }
          }
        }
        else {
          assert(fields_.size() - attr_having_begin == 1); //只有一个group by属性
          ComparisonExpr* comp = static_cast<ComparisonExpr *>(exp);
          CompOp op = comp->comp();
          if (comp->right()->type() == ExprType::FIELD){
            assert(comp->left()->type() == ExprType::VALUE);
            std::unique_ptr<Expression> left = std::unique_ptr<ValueExpr>(new ValueExpr(static_cast<ValueExpr*>(comp->left().get())->get_value()));
            std::unique_ptr<Expression> right = std::unique_ptr<ValueExpr>(new ValueExpr(values[group_by_begin]));//having_funcs起始位置在正常的values值的末尾+1处
            ComparisonExpr temp = ComparisonExpr(op, std::move(left), std::move(right));
            Value value;
            temp.get_value(*tuple, value);
            if (!value.get_boolean()){
              flag = false;
              break;
            }
          }
          else{
            assert(comp->left()->type() == ExprType::FIELD);
            assert(comp->right()->type() == ExprType::VALUE);
            std::unique_ptr<Expression> right = std::unique_ptr<ValueExpr>(new ValueExpr(static_cast<ValueExpr*>(comp->right().get())->get_value()));
            std::unique_ptr<Expression> left = std::unique_ptr<ValueExpr>(new ValueExpr(values[group_by_begin]));
            ComparisonExpr temp = ComparisonExpr(op, std::move(left), std::move(right));
            Value value;
            temp.get_value(*tuple, value);
            if (!value.get_boolean()){
              flag = false;
            }
          }
        }
      }
      if (flag) {
        int cell_num = attr_having_begin == -1 ? values.size() : group_by_begin; //having_funcs起始位置在正常的values值的末尾+1处
        tuple->set_cells_copy(std::vector<Value>(values.begin(), values.begin() + cell_num));
        tuples_.push_back(tuple);
      }
    }
  }
  else
  {
    std::vector<Value> values;
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
    
    do_aggregation_(values, tuples);//, cell_num);

    for(Tuple* tuple : tuples)
      delete static_cast<ValueListTuple *>(tuple);

    ValueListTuple* tuple = new ValueListTuple();
    tuple->set_cells_copy(values);
    tuples_.push_back(tuple);
  }
}

RC AggregationPhysicalOperator::do_aggregation_(std::vector<Value> &values, std::vector<ValueListTuple *> &tuples){//, int cell_num){
  values.clear();
  if (tuples.size() == 0)
    return RC::SUCCESS;
  int cell_num = tuples[0]->cell_num();
  values.resize(cell_num);

  int avg_size = 0;
  for (auto iter = funcs_.begin(); iter != funcs_.end(); iter ++){
    if (*iter == AggregationFunc::AVGFUN)
      avg_size ++;
  }
  std::vector<int> num(avg_size, 0);

  for(Tuple* tuple : tuples){
    auto iter_avg_num = num.begin();
    //int cell_num = tuple->cell_num();
    for (int i = 0; i < cell_num; i ++){
      Value value;
      RC rc = tuple->cell_at(i, value);
      if (rc != RC::SUCCESS) {
        return rc;    
      }
      if (tuple == *(tuples.begin())){
        if (funcs_[i] != AggregationFunc::COUNTFUN && funcs_[i] != AggregationFunc::AVGFUN){
          values[i] = value;
        }
        else{
          if (!(value.nullable() && value.is_null())){
            if (funcs_[i] != AggregationFunc::AVGFUN)
              values[i] = Value(1);
            else{
              values[i] = value;
              (*iter_avg_num) ++;
              iter_avg_num ++;
            }
          }
          else if (funcs_[i] == AggregationFunc::AVGFUN)
            // 接受null，便于最后平均值是否为null的判断
            values[i] = value;
          else {
            if (!fields_[i].is_star())
              values[i] = Value(0);
            else
              values[i] = Value(1);
          }
        }
      }
      else{
        switch (funcs_[i]){
          case AggregationFunc::NONE: {

          } break;
          case AggregationFunc::MAXFUN: {
            if (values[i].nullable() && values[i].is_null())
              values[i] = value;
            else if (!(value.nullable() && value.is_null()) && value.compare(values[i]) > 0)
              values[i] = value;
          } break;
          case AggregationFunc::MINFUN: {
            if (values[i].nullable() && values[i].is_null())
              values[i] = value;
            else if (!(value.nullable() && value.is_null()) && value.compare(values[i]) < 0)
              values[i] = value;
          } break;
          case AggregationFunc::COUNTFUN: {
            if (!(value.nullable() && value.is_null()))
              values[i].set_int(values[i].get_int() + 1);
            else{
              if (fields_[i].is_star())              
                values[i].set_int(values[i].get_int() + 1);
            }
          } break;
          case AggregationFunc::AVGFUN: {
            if (!(value.nullable() && value.is_null())){
              (*iter_avg_num) ++;
              iter_avg_num ++;
              switch (value.attr_type()) {
                // INTS时，最后的结果放在Value.float_value_
                case AttrType::INTS: {
                  values[i].set_int(values[i].get_int() + value.get_int());
                } break;
                case AttrType::FLOATS: {
                  values[i].set_float(values[i].get_float() + value.get_float());
                } break;
                default: {
                  LOG_WARN("invalid type to calculate average: %s", fields_[i].field_name());
                  return RC::INVALID_ARGUMENT;
                } break;
              }
            } 
          } break;
          case AggregationFunc::SUMFUN: {
            if (!(value.nullable() && value.is_null())){
              switch (value.attr_type()) {
                // INTS时，最后的结果放在Value.float_value_
                case AttrType::INTS: {
                  values[i].set_int(values[i].get_int() + value.get_int());
                } break;
                case AttrType::FLOATS: {
                  values[i].set_float(values[i].get_float() + value.get_float());
                } break;
                default: {
                  LOG_WARN("invalid type to calculate average: %s", fields_[i].field_name());
                  return RC::INVALID_ARGUMENT;
                } break;
              }
            }
          } break;
        }
      }
    }
  }

  // avg还没有除以总数
  auto iter_avg_num = num.begin();
  for (int i = 0; i < cell_num; i ++) {
    if (funcs_[i] == AggregationFunc::AVGFUN){
      if (!(values[i].nullable() && values[i].is_null())){
        if (values[i].attr_type() == AttrType::INTS){
          values[i].set_float(values[i].get_int());
        }
        values[i].set_float(values[i].get_float() / *iter_avg_num);
        iter_avg_num ++;
      }
    }
  }
  return RC::SUCCESS;
}