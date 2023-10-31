#pragma once

#include <string>
#include <vector>

#include "sql/stmt/stmt.h"
#include "sql/stmt/select_stmt.h"

class Db;

/**
 * @brief 表示创建表的语句
 * @ingroup Statement
 * @details 虽然解析成了stmt，但是与原始的SQL解析后的数据也差不多
 */
class CreateTableSelectStmt : public Stmt
{
public:
  CreateTableSelectStmt(const std::string &table_name, SelectStmt *select)
        : table_name_(table_name),
          select_(select)
  {}
  virtual ~CreateTableSelectStmt() = default;

  StmtType type() const override { return StmtType::CREATE_TABLE_SELECT; }

  const std::string &table_name() const { return table_name_; }

  static RC create(Db *db, const CreateTableSelectSqlNode &create_table_select, Stmt *&stmt);
  SelectStmt *select_stmt() { return select_; }

private:
  std::string table_name_;
  SelectStmt *select_;
};