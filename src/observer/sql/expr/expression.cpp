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
// Created by Wangyunlai on 2022/07/05.
//

#include "sql/expr/expression.h"
#include "sql/expr/tuple.h"
#include "common/lang/comparator.h"

#include <math.h>
#include <iomanip>
using namespace std;

RC FieldExpr::get_value(const Tuple &tuple, Value &value) const
{
  AggregationFunc func = field_.func();
  if (func == NONE)
    return tuple.find_cell(TupleCellSpec(table_name(), field_name()), value);
  else
    return static_cast<const ValueListTuple&>(tuple).find_cell(TupleCellSpec(table_name(), field_name()), func, value);
}

RC ValueExpr::get_value(const Tuple &tuple, Value &value) const
{
  value = value_;
  return RC::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////
CastExpr::CastExpr(unique_ptr<Expression> child, AttrType cast_type)
    : child_(std::move(child)), cast_type_(cast_type)
{}

CastExpr::~CastExpr()
{}

RC CastExpr::cast(const Value &value, Value &cast_value) const
{
  RC rc = RC::SUCCESS;
  if (this->value_type() == value.attr_type()) {
    cast_value = value;
    return rc;
  }

  switch (cast_type_) {
    case BOOLEANS: {
      bool val = value.get_boolean();
      cast_value.set_boolean(val);
    } break;
    default: {
      rc = RC::INTERNAL;
      LOG_WARN("unsupported convert from type %d to %d", child_->value_type(), cast_type_);
    }
  }
  return rc;
}

RC CastExpr::get_value(const Tuple &tuple, Value &cell) const
{
  RC rc = child_->get_value(tuple, cell);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  return cast(cell, cell);
}

RC CastExpr::try_get_value(Value &value) const
{
  RC rc = child_->try_get_value(value);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  return cast(value, value);
}

////////////////////////////////////////////////////////////////////////////////

ComparisonExpr::ComparisonExpr(CompOp comp, unique_ptr<Expression> left, unique_ptr<Expression> right)
    : comp_(comp), left_(std::move(left)), right_(std::move(right))
{}

ComparisonExpr::~ComparisonExpr()
{}

RC ComparisonExpr::compare_value(const Value &left, const Value &right, bool &result) const
{
  RC rc = RC::SUCCESS;
  bool left_null = left.nullable() && left.is_null();
  bool right_null = right.nullable() && right.is_null();
  bool in_equal = comp_ == IN_OP || comp_ == NOT_IN_OP;
  if (comp_ == NOT_IN_OP && right_null){
    result = true;
    return rc;
  }
  // right.is_null() == -1表示not null
  else if (right.is_null() == -1){
    if (!left_null && right_null && comp_ == EQUAL_TO){
      result = true;
      return rc;
    }
    else if(left_null && right_null && comp_ == EQUAL_TO){
      result = false;
      return rc;
    }
  }
  else if (left_null && right_null && comp_ == EQUAL_TO){
    if (left.attr_type() != UNDEFINED && right.attr_type() != UNDEFINED) {
      result = false;
      return rc;
    }
    result = true;
    return rc;
  }
  else if (left_null && right_null && in_equal){
    result = false;
    return rc;
  }
  else if (left_null || right_null){
    result = false;
    return rc;
  }
    
  int cmp_result = left.compare(right);
  result = false;
  switch (comp_) {
    case EQUAL_TO: case IN_OP: case NOT_IN_OP: {
      result = (0 == cmp_result);
    } break;
    case LESS_EQUAL: {
      result = (cmp_result <= 0);
    } break;
    case NOT_EQUAL: {
      result = (cmp_result != 0);
    } break;
    case LESS_THAN: {
      result = (cmp_result < 0);
    } break;
    case GREAT_EQUAL: {
      result = (cmp_result >= 0);
    } break;
    case GREAT_THAN: {
      result = (cmp_result > 0);
    } break;
    case LIKE_A: {
      // 实现char的like匹配
      cmp_result = common::compare_like((void *)left.data(), (void *)right.data());
      result = (1 == cmp_result);
    } break;
    case LIKE_NOT: {
        // NOT LIKE
        cmp_result = common::compare_like((void *)left.data(), (void *)right.data());
        result = (0 == cmp_result);
    } break;
    default: {
      LOG_WARN("unsupported comparison. %d", comp_);
      rc = RC::INTERNAL;
    } break;
  }

  return rc;
}

RC ComparisonExpr::compare_value(const Value &left, std::vector<Value> &collection, bool &value) const
{
  RC rc = RC::SUCCESS;
  if (comp_ == EXISTS_OP) {
    value = !collection.empty();
    return rc;
  }
  if (comp_ == NOT_EXISTS_OP) {
    value = collection.empty();
    return rc;
  }
  if (comp_ < IN_OP && comp_ >= EQUAL_TO) {
    if (collection.size() == 0) {
      value = false;
      return rc;
    }
    else if (collection.size() > 1) {
      value = false;
      return RC::INTERNAL;
    }
    return compare_value(left, collection[0], value);
  }

  value = comp_ == IN_OP;
  for (Value &right : collection) {
    bool cmp_result;
    rc = compare_value(left, right, cmp_result);
    if (OB_FAIL(rc)) {
      return rc;
    }
    if (cmp_result) {
      return rc;
    }
  }
  value = !value;
  return rc;
}

RC ComparisonExpr::try_get_value(Value &cell) const
{
  if (left_->type() == ExprType::VALUE && right_->type() == ExprType::VALUE) {
    ValueExpr *left_value_expr = static_cast<ValueExpr *>(left_.get());
    ValueExpr *right_value_expr = static_cast<ValueExpr *>(right_.get());
    const Value &left_cell = left_value_expr->get_value();
    const Value &right_cell = right_value_expr->get_value();

    bool value = false;
    RC rc = compare_value(left_cell, right_cell, value);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to compare tuple cells. rc=%s", strrc(rc));
    } else {
      cell.set_boolean(value);
    }
    return rc;
  }

  return RC::INVALID_ARGUMENT;
}

RC ComparisonExpr::get_value(const Tuple &tuple, Value &value) const
{
  Value left_value;
  Value right_value;
  std::vector<Value> right_values;

  RC rc = left_->get_value(tuple, left_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of left expression. rc=%s", strrc(rc));
    return rc;
  }
  rc = right_->get_value(tuple, right_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of right expression. rc=%s", strrc(rc));
    return rc;
  }

  bool bool_value = false;

  if (right_->type() == ExprType::COLLECTION) {
    right_->try_get_value(right_values);
    rc = compare_value(left_value, right_values, bool_value);
    if (rc == RC::SUCCESS) {
      value.set_boolean(bool_value);
    }
    return rc;
  }

  
  rc = compare_value(left_value, right_value, bool_value);
  if (rc == RC::SUCCESS) {
    value.set_boolean(bool_value);
  }
  return rc;
}


RC ComparisonExpr::get_value(const Tuple &tuple, Value &value, std::vector<Value> &collection) const
{
  Value left_value;
  RC rc = left_->get_value(tuple, left_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of left expression. rc=%s", strrc(rc));
    return rc;
  }

  bool bool_value = false;
  rc = compare_value(left_value, collection, bool_value);
  if (rc == RC::SUCCESS) {
    value.set_boolean(bool_value);
  }
  return rc;

}

////////////////////////////////////////////////////////////////////////////////
ConjunctionExpr::ConjunctionExpr(Type type, vector<unique_ptr<Expression>> &children)
    : conjunction_type_(type), children_(std::move(children))
{}

RC ConjunctionExpr::get_value(const Tuple &tuple, Value &value) const
{
  RC rc = RC::SUCCESS;
  if (children_.empty()) {
    value.set_boolean(true);
    return rc;
  }

  Value tmp_value;
  for (const unique_ptr<Expression> &expr : children_) {
    rc = expr->get_value(tuple, tmp_value);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to get value by child expression. rc=%s", strrc(rc));
      return rc;
    }
    bool bool_value = tmp_value.get_boolean();
    if ((conjunction_type_ == Type::AND && !bool_value) || (conjunction_type_ == Type::OR && bool_value)) {
      value.set_boolean(bool_value);
      return rc;
    }
  }

  bool default_value = (conjunction_type_ == Type::AND);
  value.set_boolean(default_value);
  return rc;
}

////////////////////////////////////////////////////////////////////////////////

ArithmeticExpr::ArithmeticExpr(ArithmeticExpr::Type type, Expression *left, Expression *right)
    : arithmetic_type_(type), left_(left), right_(right)
{}
ArithmeticExpr::ArithmeticExpr(ArithmeticExpr::Type type, unique_ptr<Expression> left, unique_ptr<Expression> right)
    : arithmetic_type_(type), left_(std::move(left)), right_(std::move(right))
{}

AttrType ArithmeticExpr::value_type() const
{
  if (!right_) {
    return left_->value_type();
  }

  if (left_->value_type() == AttrType::INTS &&
      right_->value_type() == AttrType::INTS &&
      arithmetic_type_ != Type::DIV) {
    return AttrType::INTS;
  }
  
  return AttrType::FLOATS;
}

RC ArithmeticExpr::calc_value(const Value &left_value, const Value &right_value, Value &value) const
{
  RC rc = RC::SUCCESS;

  const AttrType target_type = value_type();

  switch (arithmetic_type_) {
    case Type::ADD: {
      if (target_type == AttrType::INTS) {
        value.set_int(left_value.get_int() + right_value.get_int());
      } else {
        value.set_float(left_value.get_float() + right_value.get_float());
      }
    } break;

    case Type::SUB: {
      if (target_type == AttrType::INTS) {
        value.set_int(left_value.get_int() - right_value.get_int());
      } else {
        value.set_float(left_value.get_float() - right_value.get_float());
      }
    } break;

    case Type::MUL: {
      if (target_type == AttrType::INTS) {
        value.set_int(left_value.get_int() * right_value.get_int());
      } else {
        value.set_float(left_value.get_float() * right_value.get_float());
      }
    } break;

    case Type::DIV: {
      if (target_type == AttrType::INTS) {
        if (right_value.get_int() == 0) {
          // NOTE: 设置为整数最大值是不正确的。通常的做法是设置为NULL，但是当前的miniob没有NULL概念，所以这里设置为整数最大值。
          value.set_int(numeric_limits<int>::max());
        } else {
          value.set_int(left_value.get_int() / right_value.get_int());
        }
      } else {
        if (right_value.get_float() > -EPSILON && right_value.get_float() < EPSILON) {
          // NOTE: 设置为浮点数最大值是不正确的。通常的做法是设置为NULL，但是当前的miniob没有NULL概念，所以这里设置为浮点数最大值。
          value.set_float(numeric_limits<float>::max());
        } else {
          value.set_float(left_value.get_float() / right_value.get_float());
        }
      }
    } break;

    case Type::NEGATIVE: {
      if (target_type == AttrType::INTS) {
        value.set_int(-left_value.get_int());
      } else {
        value.set_float(-left_value.get_float());
      }
    } break;

    default: {
      rc = RC::INTERNAL;
      LOG_WARN("unsupported arithmetic type. %d", arithmetic_type_);
    } break;
  }
  return rc;
}

RC ArithmeticExpr::get_value(const Tuple &tuple, Value &value) const
{
  RC rc = RC::SUCCESS;

  Value left_value;
  Value right_value;

  rc = left_->get_value(tuple, left_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of left expression. rc=%s", strrc(rc));
    return rc;
  }
  rc = right_->get_value(tuple, right_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of right expression. rc=%s", strrc(rc));
    return rc;
  }
  return calc_value(left_value, right_value, value);
}

RC ArithmeticExpr::try_get_value(Value &value) const
{
  RC rc = RC::SUCCESS;

  Value left_value;
  Value right_value;

  rc = left_->try_get_value(left_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of left expression. rc=%s", strrc(rc));
    return rc;
  }

  if (right_) {
    rc = right_->try_get_value(right_value);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to get value of right expression. rc=%s", strrc(rc));
      return rc;
    }
  }

  return calc_value(left_value, right_value, value);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
AttrType FunctionExpr::value_type() const
{
  switch (function_type_)
  {
  case Type::LENGTH:
    return AttrType::INTS;

  case Type::ROUND:
    return AttrType::FLOATS;

  case Type::DATE_FORMAT:
    return AttrType::CHARS;

  default:
    return AttrType::UNDEFINED;
  }
}

RC FunctionExpr::length(Value &value) const
{
  if (value.attr_type() != AttrType::CHARS) {
    return RC::INVALID_ARGUMENT;
  }
  // TODO deal with null
  int len = value.get_string().size();
  if (value.nullable())
    len --;

  value.set_int(len);
  return RC::SUCCESS;
}

RC FunctionExpr::round(Value &value) const
{
  if (value.attr_type() != AttrType::FLOATS) {
    return RC::INVALID_ARGUMENT;
  }
  float f = value.get_float();
  
  std::stringstream ss;
  ss.setf(ios::fixed);
  f += 1 / pow(10, round_ + 1);
  ss.precision(round_);
  ss << fixed << f;
  ss >> f;
  value.set_float(f);
  return RC::SUCCESS;

}

RC FunctionExpr::date_format(Value &value) const
{
  if (value.attr_type() != AttrType::DATES) {
    return RC::INVALID_ARGUMENT;
  }

  tm date_struct;
  Date date_t = value.get_date();
  int date = date_t.value();
  int day = date % 100;
  int last_digit_of_day = day % 10;
  date /= 100;
  int month = date % 100;
  date /= 100;
  int year = date;

  std::stringstream formatted_date;

  for (int i = 0; i < format_.size(); i++) {
    if (format_[i] != '%') {
      formatted_date << format_[i];
      continue;
    } 
    if (++i >= format_.size()) {
      formatted_date << format_[i - 1];
      break;
    }
    switch (format_[i])
    {
    case 'b':
      formatted_date << month_name[month - 1].substr(0, 3);
      break;
    case 'c':
      formatted_date << month;
      break;
    case 'D': {
      std::string tmp;
      if (last_digit_of_day == 1) 
        tmp = "st";
      else if (last_digit_of_day == 2)
        tmp = "nd";
      else if (last_digit_of_day == 3)
        tmp = "rd";
      else 
        tmp = "th";
      formatted_date << day << tmp;
    } break;
    case 'd':
      formatted_date << setw(2) << setfill('0') << day;
      break;
    case 'e':
      formatted_date << day;
      break;
    case 'j': {
      int sum_of_day = 0;
      int leap_year = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
      for (int i = 0; i < month; i++) {
        sum_of_day += month_day[i];
        if (i == 1) {
          sum_of_day += leap_year;
        }
      }
      formatted_date << setw(3) << setfill('0') << sum_of_day;
    } break;
    case 'M':
      formatted_date << month_name[month - 1];
      break;
    case 'm':
      formatted_date << setw(2) << setfill('0') << month;
      break;
    case 'Y':
       formatted_date << year;
       break;
    case 'y':
        formatted_date << setw(2) << setfill('0') << year % 100;
       break;
    default:
        formatted_date << format_[i];
      break;
    }
  }


  value.set_string(formatted_date.str().c_str());
  return RC::SUCCESS;

}

RC FunctionExpr::try_get_value(Value &value) const 
{
  RC rc = expr_->try_get_value(value);
  if (OB_FAIL(rc)) {
    return rc;
  }
  switch (function_type_)
  {
  case Type::LENGTH:
    return length(value);
  case Type::ROUND:
    return round(value);
  case Type::DATE_FORMAT:
    return date_format(value);
  
  default:
    return RC::INVALID_ARGUMENT;
  }
}

RC FunctionExpr::get_value(const Tuple &tuple, Value &value) const
{
  RC rc = expr_->get_value(tuple, value);
  if (OB_FAIL(rc)) {
    return rc;
  }
  switch (function_type_)
  {
  case Type::LENGTH:
    return length(value);
  case Type::ROUND:
    return round(value);
  case Type::DATE_FORMAT:
    return date_format(value);
  
  default:
    return RC::INVALID_ARGUMENT;
  }
  
}