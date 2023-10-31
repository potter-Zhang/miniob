
#include "sql/stmt/create_unique_index_stmt.h"
#include "sql/parser/parse_defs.h"
#include "event/sql_debug.h"

#include "storage/table/table.h"
#include "storage/db/db.h"
#include "common/lang/string.h"
#include "common/log/log.h"

using namespace std;
using namespace common;

RC CreateUniqueIndexStmt::create(Db *db, const CreateUniqueIndexSqlNode &create_index, Stmt *&stmt)
{
  stmt = nullptr;

  const char *table_name = create_index.relation_name.c_str();
  if (is_blank(table_name) || is_blank(create_index.index_name.c_str())) {
    LOG_WARN("invalid argument. db=%p, table_name=%p, index name=%s",
        db, table_name, create_index.index_name.c_str());
    return RC::INVALID_ARGUMENT;
  }

  for (const std::string &attr : create_index.attribute_name) {
    if (is_blank(attr.c_str())) {
        return RC::INVALID_ARGUMENT;
    }
  }

  // check whether the table exists
  Table *table = db->find_table(table_name);
  if (nullptr == table) {
    LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }
  std::vector<const FieldMeta *> field_metas;
  for (const std::string &attr : create_index.attribute_name) {
    const FieldMeta *field_meta = table->table_meta().field(attr.c_str());
    field_metas.emplace_back(field_meta);
    if (nullptr == field_meta) {
        LOG_WARN("no such field in table. db=%s, table=%s, field name=%s", 
             db->name(), table_name, attr.c_str());
    return RC::SCHEMA_FIELD_NOT_EXIST;   
    }
  }
  

  Index *index = table->find_index(create_index.index_name.c_str());
  if (nullptr != index) {
    LOG_WARN("index with name(%s) already exists. table name=%s", create_index.index_name.c_str(), table_name);
    return RC::SCHEMA_INDEX_NAME_REPEAT;
  }

  stmt = new CreateUniqueIndexStmt(table, field_metas, create_index.index_name);
  return RC::SUCCESS;
}
