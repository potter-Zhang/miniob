#include "sql/executor/create_multi_index_executor.h"
#include "sql/stmt/create_multi_index_stmt.h"
#include "event/sql_event.h"
#include "event/session_event.h"
#include "session/session.h"
#include "common/log/log.h"
#include "storage/table/table.h"

RC CreateMultiIndexExecutor::execute(SQLStageEvent *sql_event)
{
  Stmt *stmt = sql_event->stmt();
  Session *session = sql_event->session_event()->session();
  ASSERT(stmt->type() == StmtType::CREATE_MULTIINDEX, 
         "create index executor can not run this command: %d", static_cast<int>(stmt->type()));

  CreateMultiIndexStmt *create_multi_index_stmt = static_cast<CreateMultiIndexStmt *>(stmt);
  
  Trx *trx = session->current_trx();
  Table *table = create_multi_index_stmt->table();
  return RC::SUCCESS;
  //return table->create_multi_index_stmt(trx, create_multi_index_stmt->field_meta(), create_multi_index_stmt->index_name().c_str());
}