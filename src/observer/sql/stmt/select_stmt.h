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
// Created by Wangyunlai on 2022/6/5.
//

#pragma once

#include <vector>
#include <memory>
#include <unordered_map> 

#include "common/rc.h"
#include "sql/stmt/stmt.h"
#include "storage/field/field.h"
#include "sql/stmt/stmt.h"
#include "sql/expr/expression.h"

class FieldMeta;
class FilterStmt;
class CalcStmt;
class Db;
class Table;

/**
 * @brief 表示select语句
 * @ingroup Statement
 */
class SelectStmt : public Stmt 
{
public:
  SelectStmt() = default;
  ~SelectStmt() override;

  StmtType type() const override
  {
    return StmtType::SELECT;
  }

public:
  static RC create(Db *db, const SelectSqlNode &select_sql, Stmt *&stmt);
  static RC create(Db *db, const SelectSqlNode &select_sql, Stmt *&stmt, std::unordered_map<std::string, std::string> &table_alias);

public:
  const std::vector<Table *> &tables() const
  {
    return tables_;
  }
  const std::vector<Field> &query_fields() const
  {
    return query_fields_;
  }
  const std::vector<bool> &is_asc() const
  {
    return is_asc_;
  }
  FilterStmt *filter_stmt() const
  {
    return filter_stmt_;
  }
  const int group_by_begin() const
  {
    return group_by_begin_;
  }
  const int having_begin() const
  {
    return having_begin_;
  }
  const int attr_having_begin() const
  {
    return attr_having_begin_;
  }
  const int order_by_begin() const
  {
    return order_by_begin_;
  }
  const std::unordered_map<std::string, std::string> &table_alias() const
  {
    return table_alias_;
  }
  const std::vector<std::string> &column_alias() const
  {
    return column_alias_;
  }
  std::vector<std::unique_ptr<Expression>> &expressions() 
  {
    return expressions_;
  }

  static RC set_up_expression(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables, std::unique_ptr<Expression> &expr);
  
 

private:
  std::vector<Field> query_fields_;
  std::unordered_map<std::string, std::string> table_alias_;
  std::vector<std::string> column_alias_;
  std::vector<Table *> tables_;
  std::vector<bool> is_asc_;
  FilterStmt *filter_stmt_ = nullptr;
  int group_by_begin_ = -1;
  int having_begin_ = -1;
  int attr_having_begin_ = -1;
  int order_by_begin_ = -1;
  std::vector<std::unique_ptr<Expression>> expressions_;
};
