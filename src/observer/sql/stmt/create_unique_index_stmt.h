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
// Created by Wangyunlai on 2023/4/25.
//

#pragma once

#include <string>

#include "sql/stmt/stmt.h"

struct CreateIndexSqlNode;
class Table;
class FieldMeta;

/**
 * @brief 创建索引的语句
 * @ingroup Statement
 */
class CreateUniqueIndexStmt : public Stmt
{
public:
  CreateUniqueIndexStmt(Table *table, std::vector<const FieldMeta *>&field_metas, const std::string &index_name)
        : table_(table),
          field_metas_(field_metas),
          index_name_(index_name)
  {}

  virtual ~CreateUniqueIndexStmt() = default;

  StmtType type() const override { return StmtType::CREATE_UNIQUE_INDEX; }

  Table *table() const { return table_; }
  std::vector<const FieldMeta*> & field_metas() { return field_metas_; }
  const std::string &index_name() const { return index_name_; }

public:
  static RC create(Db *db, const CreateUniqueIndexSqlNode &create_index, Stmt *&stmt);

private:
  Table *table_ = nullptr;
  std::vector<const FieldMeta *> field_metas_ ;
  std::string index_name_;
};
