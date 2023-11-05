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

#pragma once

#include <string.h>
#include <memory>
#include <string>

#include "storage/field/field.h"
#include "sql/parser/value.h"
#include "sql/stmt/select_stmt.h"
#include "common/log/log.h"
#include "storage/db/db.h"
#include "common/rc.h"
#include "common/log/log.h"
#include "common/lang/string.h"
#include "event/sql_debug.h"
//#include "sql/operator/logical_operator.h"

class Tuple;

/**
 * @defgroup Expression
 * @brief 表达式
 */

/**
 * @brief 表达式类型
 * @ingroup Expression
 */
enum class ExprType 
{
  NONE,
  STAR,         ///< 星号，表示所有字段
  FIELD,        ///< 字段。在实际执行时，根据行数据内容提取对应字段的值
  VALUE,        ///< 常量值
  CAST,         ///< 需要做类型转换的表达式
  COMPARISON,   ///< 需要做比较的表达式
  CONJUNCTION,  ///< 多个表达式使用同一种关系(AND或OR)来联结
  ARITHMETIC,   ///< 算术运算
  COLLECTION,    ///< 
  FUNCTION,
  STMT,
  EMPTY
};

/**
 * @brief 表达式的抽象描述
 * @ingroup Expression
 * @details 在SQL的元素中，任何需要得出值的元素都可以使用表达式来描述
 * 比如获取某个字段的值、比较运算、类型转换
 * 当然还有一些当前没有实现的表达式，比如算术运算。
 *
 * 通常表达式的值，是在真实的算子运算过程中，拿到具体的tuple后
 * 才能计算出来真实的值。但是有些表达式可能就表示某一个固定的
 * 值，比如ValueExpr。
 */
class Expression 
{
public:
  Expression() = default;
  virtual ~Expression() = default;

  /**
   * @brief 根据具体的tuple，来计算当前表达式的值。tuple有可能是一个具体某个表的行数据
   */
  virtual RC get_value(const Tuple &tuple, Value &value) const = 0;

  /**
   * @brief 在没有实际运行的情况下，也就是无法获取tuple的情况下，尝试获取表达式的值
   * @details 有些表达式的值是固定的，比如ValueExpr，这种情况下可以直接获取值
   */
  virtual RC try_get_value(Value &value) const
  {
    return RC::UNIMPLENMENT;
  }

  virtual RC get_value(const Tuple &tuple, Value &value, std::vector<Value> &collection) const
  {
    return RC::UNIMPLENMENT;
  }

  virtual RC try_get_value(std::vector<Value> &collection) const
  {
    return RC::UNIMPLENMENT;
  }

  /**
   * @brief 表达式的类型
   * 可以根据表达式类型来转换为具体的子类
   */
  virtual ExprType type() const = 0;

  /**
   * @brief 表达式值的类型
   * @details 一个表达式运算出结果后，只有一个值
   */
  virtual AttrType value_type() const = 0;

  /**
   * @brief 表达式的名字，比如是字段名称，或者用户在执行SQL语句时输入的内容
   */
  virtual std::string name() const { return name_; }
  virtual void set_name(std::string name) { name_ = name; }

private:
  std::string  name_;
};

/**
 * @brief 字段表达式
 * @ingroup Expression
 */
class FieldExpr : public Expression 
{
public:
  FieldExpr() = default;
  FieldExpr(const Table *table, const FieldMeta *field) : field_(table, field)
  {}
  FieldExpr(const Field &field) : field_(field)
  {}
  FieldExpr(const RelAttrSqlNode &rel) : rel_(rel)
  {}

  virtual ~FieldExpr() = default;

  void set_table_field(const Table *table, const FieldMeta *field) { field_ = Field(table, field); }
  RC get_table_and_field(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables)
  {
    Table *table;
    const FieldMeta *field;
    if (common::is_blank(rel_.relation_name.c_str())) {
      table = default_table;
    } else if (nullptr != tables) {
      auto iter = tables->find(rel_.relation_name);
      if (iter != tables->end()) {
        table = iter->second;
      }
    } else {
      table = db->find_table(rel_.relation_name.c_str());
    }
    if (nullptr == table) {
      
      return RC::SCHEMA_TABLE_NOT_EXIST;
    }

    field = table->table_meta().field(rel_.attribute_name.c_str());
    if (nullptr == field && rel_.attribute_name != "*") {
      
      table = nullptr;
      return RC::SCHEMA_FIELD_NOT_EXIST;
    }
    field_ = Field(table, field);
    if (field_.func() == NONE)
      field_.set_func(rel_.func);
    return RC::SUCCESS;
  }
  ExprType type() const override { return ExprType::FIELD; }
  AttrType value_type() const override { return field_.attr_type(); }

  RelAttrSqlNode &rel() { return rel_; }

  Field &field() { return field_; }

  const Field &field() const { return field_; }

  const char *table_name() const { return field_.table_name(); }

  const char *field_name() const { return field_.field_name(); }

  RC get_value(const Tuple &tuple, Value &value) const override;

  void set_func(AggregationFunc func) {
    field_.set_func(func);
  }

private:
  Field field_;
  RelAttrSqlNode rel_;
};

/**
 * @brief 常量值表达式
 * @ingroup Expression
 */
class ValueExpr : public Expression 
{
public:
  ValueExpr() = default;
  explicit ValueExpr(const Value &value) : value_(value)
  {}

  virtual ~ValueExpr() = default;

  RC get_value(const Tuple &tuple, Value &value) const override;
  RC try_get_value(Value &value) const override { value = value_; return RC::SUCCESS; }

  ExprType type() const override { return ExprType::VALUE; }

  AttrType value_type() const override { return value_.attr_type(); }

  void get_value(Value &value) const { value = value_; }

  const Value &get_value() const { return value_; }

  bool try_convert_attr_type(AttrType newAttrType) { return value_.convert_to(newAttrType); }

private:
  Value value_;
};

/**
 * @brief 类型转换表达式
 * @ingroup Expression
 */
class CastExpr : public Expression 
{
public:
  CastExpr(std::unique_ptr<Expression> child, AttrType cast_type);
  virtual ~CastExpr();

  ExprType type() const override
  {
    return ExprType::CAST;
  }
  RC get_value(const Tuple &tuple, Value &value) const override;

  RC try_get_value(Value &value) const override;

  AttrType value_type() const override { return cast_type_; }

  std::unique_ptr<Expression> &child() { return child_; }

private:
  RC cast(const Value &value, Value &cast_value) const;

private:
  std::unique_ptr<Expression> child_;  ///< 从这个表达式转换
  AttrType cast_type_;  ///< 想要转换成这个类型
};

/**
 * @brief 比较表达式
 * @ingroup Expression
 */
class ComparisonExpr : public Expression 
{
public:
  ComparisonExpr(CompOp comp, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right);
  virtual ~ComparisonExpr();

  ExprType type() const override { return ExprType::COMPARISON; }

  RC get_value(const Tuple &tuple, Value &value) const override;

  AttrType value_type() const override { return BOOLEANS; }

  CompOp comp() const { return comp_; }

  std::unique_ptr<Expression> &left()  { return left_;  }
  std::unique_ptr<Expression> &right() { return right_; }

  /**
   * 尝试在没有tuple的情况下获取当前表达式的值
   * 在优化的时候，可能会使用到
   */
  RC try_get_value(Value &value) const override;

  /**
   * compare the two tuple cells
   * @param value the result of comparison
   */
  RC compare_value(const Value &left, const Value &right, bool &value) const;

  RC get_value(const Tuple &tuple, Value &value, std::vector<Value> &collection) const override;
  RC compare_value(const Value &left, std::vector<Value> &collection, bool &value) const;
  

private:
  CompOp comp_;
  std::unique_ptr<Expression> left_;
  std::unique_ptr<Expression> right_;
};

/**
 * @brief 联结表达式
 * @ingroup Expression
 * 多个表达式使用同一种关系(AND或OR)来联结
 * 当前miniob仅有AND操作
 */
class ConjunctionExpr : public Expression 
{
public:
  enum class Type {
    AND,
    OR,
  };

public:
  ConjunctionExpr(Type type, std::vector<std::unique_ptr<Expression>> &children);
  virtual ~ConjunctionExpr() = default;

  ExprType type() const override { return ExprType::CONJUNCTION; }

  AttrType value_type() const override { return BOOLEANS; }

  RC get_value(const Tuple &tuple, Value &value) const override;

  Type conjunction_type() const { return conjunction_type_; }

  std::vector<std::unique_ptr<Expression>> &children() { return children_; }

private:
  Type conjunction_type_;
  std::vector<std::unique_ptr<Expression>> children_;
};

/**
 * @brief 算术表达式
 * @ingroup Expression
 */
class ArithmeticExpr : public Expression 
{
public:
  enum class Type {
    ADD,
    SUB,
    MUL,
    DIV,
    NEGATIVE,
  };

public:
  ArithmeticExpr(Type type, Expression *left, Expression *right);
  ArithmeticExpr(Type type, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right);
  virtual ~ArithmeticExpr() = default;

  ExprType type() const override { return ExprType::ARITHMETIC; }

  AttrType value_type() const override;

  RC get_value(const Tuple &tuple, Value &value) const override;
  RC try_get_value(Value &value) const override;

  Type arithmetic_type() const { return arithmetic_type_; }

  std::unique_ptr<Expression> &left() { return left_; }
  std::unique_ptr<Expression> &right() { return right_; }

private:
  RC calc_value(const Value &left_value, const Value &right_value, Value &value) const;
  
private:
  Type arithmetic_type_;
  std::unique_ptr<Expression> left_;
  std::unique_ptr<Expression> right_;
};

class EmptyExpr : public Expression {
public:
  EmptyExpr() = default;

  virtual ~EmptyExpr() = default;

  RC get_value(const Tuple &tuple, Value &value) const override { return RC::SUCCESS; }
  RC try_get_value(Value &value) const override { return RC::SUCCESS; }
  ExprType type() const override { return ExprType::EMPTY; }
  AttrType value_type() const override { return AttrType::UNDEFINED; }
};

class CollectionExpr : public Expression {
public:
  CollectionExpr(const std::vector<Value> &values) {
    this->values = values;
  }

  virtual ~CollectionExpr() = default;

  RC try_get_value(std::vector<Value> &value) const override { value = this->values; return RC::SUCCESS; }
  RC get_value(const Tuple &tuple, Value &value) const override { return RC::SUCCESS; }
  ExprType type() const override { return ExprType::COLLECTION; }
  AttrType value_type() const override { return AttrType::UNDEFINED; }

private:
  std::vector<Value> values;
};

class FunctionExpr : public Expression {
public:
  enum class Type {
    LENGTH,
    ROUND,
    DATE_FORMAT
  };
 static constexpr int month_day[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
 
public:
  FunctionExpr(Type type, Expression *expr) : function_type_(type), expr_(expr) {}
  FunctionExpr(Type type, Expression *expr, std::string &format) : function_type_(type), expr_(expr), format_(format) {}
  FunctionExpr(Type type, Expression *expr, int round) : function_type_(type), expr_(expr), round_(round) {}

  virtual ~FunctionExpr() = default;

  ExprType type() const override { return ExprType::FUNCTION; }

  AttrType value_type() const override;

  RC get_value(const Tuple &tuple, Value &value) const override;
  RC try_get_value(Value &value) const override;
  std::unique_ptr<Expression> &expr() { return expr_; }
  Type function_type() const { return function_type_; }

private:
  // real functions
  RC length(Value &value) const;
  RC round(Value &value) const;
  RC date_format(Value &value) const;

  
private:
  Type function_type_;
  std::unique_ptr<Expression> expr_;
  std::string format_;
  int round_;
};

const std::string month_name[] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

class StmtExpr : public Expression {
  StmtExpr(SelectSqlNode select) :select_(select) {}



private:
  SelectSqlNode select_;
  
};