#include "sql/stmt/create_table_select_stmt.h"
#include "event/sql_debug.h"

RC CreateTableSelectStmt::create(Db *db, const CreateTableSelectSqlNode &create_table_select, Stmt *&stmt)
{
  Stmt *select;
  RC rc = SelectStmt::create(db, *static_cast<const SelectSqlNode *>(create_table_select.select), select);
  if (OB_FAIL(rc)) {
    return rc;
  }
  stmt = new CreateTableSelectStmt(create_table_select.relation_name, static_cast<SelectStmt *>(select));
  sql_debug("create table statement: table name %s", create_table_select.relation_name.c_str());
  return RC::SUCCESS;
}