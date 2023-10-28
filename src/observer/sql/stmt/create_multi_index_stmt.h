#pragma once

#include <string>

#include "sql/stmt/stmt.h"

struct CreateMultiIndexSqlNode;
class Table;
class FieldMeta;

/**
 * @brief 创建索引的语句
 * @ingroup Statement
 */
class CreateMultiIndexStmt : public Stmt
{
public:
  CreateMultiIndexStmt(Table *table,  const FieldMeta *field_meta, const std::string &index_name)
        : table_(table),
          field_meta_(field_meta),
          index_name_(index_name)
  {}

  virtual ~CreateMultiIndexStmt() = default;

  StmtType type() const override { return StmtType::CREATE_MULTIINDEX; }

  Table *table() const { return table_; }
  const FieldMeta *field_meta() const { return field_meta_; }
  const std::string &index_name() const { return index_name_; }

public:
  static RC create(Db *db, const CreateMultiIndexSqlNode &create_index, Stmt *&stmt);

private:
  Table *table_ = nullptr;
  const FieldMeta *field_meta_ = nullptr;
  std::string index_name_;
};