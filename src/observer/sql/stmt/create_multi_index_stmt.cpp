
#include "sql/stmt/create_multi_index_stmt.h"
#include "storage/table/table.h"
#include "storage/db/db.h"
#include "common/lang/string.h"
#include "common/log/log.h"

using namespace std;
using namespace common;

RC CreateMultiIndexStmt::create(Db *db, const CreateMultiIndexSqlNode &create_index, Stmt *&stmt)
{
  // stmt = nullptr;
  // const char *table_name = create_index.relation_name.c_str();
  // Table *table = db->find_table(table_name);
  // stmt = new CreateMultiIndexStmt()

  const char *table_name = create_index.relation_name.c_str();
  // attribute_name 要改，是vector<string> 而不是单纯string
  if (is_blank(table_name) || is_blank(create_index.index_name.c_str()) || create_index.attr_list.empty()) {
    LOG_WARN("invalid argument. db=%p, table_name=%p, index name=%s, attribute name=%s",
        db, table_name, create_index.index_name.c_str(), create_index.attr_list);
    return RC::INVALID_ARGUMENT;
  }

  // check whether the table exists
  Table *table = db->find_table(table_name);
  if (nullptr == table) {
    LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

    // 查找是否有这些属性
  const FieldMeta *field_meta = table->table_meta().field(create_index.attr_list[0].c_str());
  
  if (nullptr == field_meta) {
    LOG_WARN("no such field in table. db=%s, table=%s, field name=%s", 
             db->name(), table_name, create_index.attr_list[0].c_str());
    return RC::SCHEMA_FIELD_NOT_EXIST;   
  }

  Index *index = table->find_index(create_index.index_name.c_str());
  if (nullptr != index) {
    LOG_WARN("index with name(%s) already exists. table name=%s", create_index.index_name.c_str(), table_name);
    return RC::SCHEMA_INDEX_NAME_REPEAT;
  }

  stmt = new CreateMultiIndexStmt(table, field_meta, create_index.index_name);
  return RC::SUCCESS;
}