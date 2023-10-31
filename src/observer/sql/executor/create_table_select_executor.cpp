#include "sql/executor/create_table_select_executor.h"

#include "session/session.h"
#include "common/log/log.h"
#include "storage/table/table.h"
#include "sql/stmt/create_table_select_stmt.h"
#include "event/sql_event.h"
#include "event/session_event.h"
#include "storage/db/db.h"
#include "storage/trx/trx.h"

/**
 * unique_ptr<PhysicalOperator> &physical_operator = sql_event->physical_operator();
  ASSERT(physical_operator != nullptr, "physical operator should not be null");

  // TODO 这里也可以优化一下，是否可以让physical operator自己设置tuple schema
  TupleSchema schema;
  switch (stmt->type()) {
    case StmtType::SELECT: {
      SelectStmt *select_stmt = static_cast<SelectStmt *>(stmt);
      bool with_table_name = select_stmt->tables().size() > 1;

      for (const Field &field : select_stmt->query_fields()) {
        if (with_table_name) {
          schema.append_cell(field.table_name(), field.field_name());
        } else {
          schema.append_cell(field.field_name());
        }
      }
    } break;

    case StmtType::CALC: {
      CalcPhysicalOperator *calc_operator = static_cast<CalcPhysicalOperator *>(physical_operator.get());
      for (const unique_ptr<Expression> & expr : calc_operator->expressions()) {
        schema.append_cell(expr->name().c_str());
      }
    } break;

    case StmtType::EXPLAIN: {
      schema.append_cell("Query Plan");
    } break;
    default: {
      // 只有select返回结果
    } break;
  }

  struct AttrInfoSqlNode
{
  AttrType    type;       ///< Type of attribute
  std::string name;       ///< Attribute name
  size_t      length;     ///< Length of attribute
};

Tuple *tuple = nullptr;
  while (RC::SUCCESS == (rc = sql_result->next_tuple(tuple))) {
    assert(tuple != nullptr);

    int cell_num = tuple->cell_num();
    for (int i = 0; i < cell_num; i++) {
      if (i != 0) {
        const char *delim = " | ";
        rc = writer_->writen(delim, strlen(delim));
        if (OB_FAIL(rc)) {
          LOG_WARN("failed to send data to client. err=%s", strerror(errno));
          sql_result->close();
          return rc;
        }
      }

      Value value;
      rc = tuple->cell_at(i, value);
      if (rc != RC::SUCCESS) {
        sql_result->close();
        return rc;
      }

      std::string cell_str = value.to_string();
      rc = writer_->writen(cell_str.data(), cell_str.size());
      if (OB_FAIL(rc)) {
        LOG_WARN("failed to send data to client. err=%s", strerror(errno));
        sql_result->close();
        return rc;
      }
    }

    char newline = '\n';
    rc = writer_->writen(&newline, 1);
    if (OB_FAIL(rc)) {
      LOG_WARN("failed to send data to client. err=%s", strerror(errno));
      sql_result->close();
      return rc;
    }
  }
*/
RC CreateTableSelectExecutor::execute(SQLStageEvent *sql_event)
{
  Stmt *stmt = sql_event->stmt();
  Session *session = sql_event->session_event()->session();
  ASSERT(stmt->type() == StmtType::CREATE_TABLE_SELECT, 
         "create table executor can not run this command: %d", static_cast<int>(stmt->type()));

  CreateTableSelectStmt *create_table_select_stmt = static_cast<CreateTableSelectStmt *>(stmt);

  std::unique_ptr<PhysicalOperator> &physical_operator = sql_event->physical_operator();

  SelectStmt *select = create_table_select_stmt->select_stmt();

  std::vector<AttrInfoSqlNode> nodes;

  for (const Field& field : select->query_fields()) {
    nodes.emplace_back(AttrInfoSqlNode{field.attr_type(), std::string(field.field_name()), (size_t)field.meta()->len(), field.meta()->nullable()});
  }

  const int attribute_count = static_cast<int>(nodes.size());

  const char *table_name = create_table_select_stmt->table_name().c_str();
  RC rc = session->get_current_db()->create_table(table_name, attribute_count, nodes.data());
  Table *new_table = session->get_current_db()->find_table(table_name);
  
  SqlResult *sql_result = sql_event->session_event()->sql_result();
  sql_result->set_operator(std::move(physical_operator));
  Trx *trx = session->current_trx();

  rc = sql_result->open();
  if (OB_FAIL(rc)) {
    return rc;
  }

  Tuple *tuple = nullptr;
  std::vector<Value> values;
  while (RC::SUCCESS == (rc = sql_result->next_tuple(tuple))) {
    assert(tuple != nullptr);

    int cell_num = tuple->cell_num();
    values.resize(cell_num);
    for (int i = 0; i < cell_num; i++) {

      Value value;
      rc = tuple->cell_at(i, value);
      if (rc != RC::SUCCESS) {
        sql_result->close();
        return rc;
      }
      values[i] = value;
    }
    Record record;
    
    new_table->make_record(values.size(), values.data(), record);
    trx->insert_record(new_table, record);
  }

  sql_result->close();
  if (rc == RC::RECORD_EOF) {
    rc = RC::SUCCESS;
  }
  return rc;
}