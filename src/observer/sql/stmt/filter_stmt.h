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
// Created by Wangyunlai on 2022/5/22.
//

#pragma once

#include <vector>
#include <unordered_map>
#include "sql/parser/parse_defs.h"
#include "sql/stmt/stmt.h"
#include "sql/stmt/select_stmt.h"
#include "sql/expr/expression.h"

class Db;
class Table;
class FieldMeta;

struct FilterObj 
{
  int is_attr;
  Field field;
  Value value;
  SelectStmt *select_stmt_;
  std::vector<Value> values;
  Expression *expr;

  void init_attr(const Field &field)
  {
    is_attr = true;
    this->field = field;
  }

  void init_select_stmt(SelectStmt *select_stmt)
  {
    is_attr = 2;
    this->select_stmt_ = select_stmt;
  }

  void init_values(const std::vector<Value>& values) {
    is_attr = 3;
    this->values = values;
  }

  void init_expr(Expression *expr) {
    is_attr = 4;
    this->expr = expr;
  }

  void init_expr(Expression *expr, SelectStmt *select_stmt)
  {
    
  }

  void init_value(const Value &value)
  {
    is_attr = false;
    this->value = value;
  }
};

class FilterUnit 
{
public:
  FilterUnit() = default;
  ~FilterUnit()
  {}

  void set_comp(CompOp comp)
  {
    comp_ = comp;
  }

  CompOp comp() const
  {
    return comp_;
  }

  void set_left(const FilterObj &obj)
  {
    left_ = obj;
  }
  void set_right(const FilterObj &obj)
  {
    right_ = obj;
  }

  const FilterObj &left() const
  {
    return left_;
  }
  const FilterObj &right() const
  {
    return right_;
  }

private:
  CompOp comp_ = NO_OP;
  FilterObj left_;
  FilterObj right_;
};

/**
 * @brief Filter/谓词/过滤语句
 * @ingroup Statement
 */
class FilterStmt 
{
public:
  FilterStmt() = default;
  virtual ~FilterStmt();

public:
  const std::vector<FilterUnit *> &filter_units() const
  {
    return filter_units_;
  }

public:
  static RC create(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
      const ConditionSqlNode *conditions, int condition_num, FilterStmt *&stmt);

  static RC create(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
      const ConditionSqlNode *conditions, int condition_num, FilterStmt *&stmt, std::unordered_map<std::string, std::string> &table_alias);

  static RC create_filter_unit(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
      const ConditionSqlNode &condition, FilterUnit *&filter_unit);

  static RC create_filter_unit(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
      const ConditionSqlNode &condition, FilterUnit *&filter_unit, std::unordered_map<std::string, std::string> &table_alias);

  RC add_filter_unit(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
      const ConditionSqlNode *conditions, int condition_num);

  
  static RC set_up_expression(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables, Expression *expr);
  
private:
  std::vector<FilterUnit *> filter_units_;  // 默认当前都是AND关系
};
