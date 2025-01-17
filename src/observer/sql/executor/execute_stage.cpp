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
// Created by Longda on 2021/4/13.
//

#include <string>
#include <sstream>
#include <algorithm>

#include "sql/executor/execute_stage.h"

#include "common/log/log.h"
#include "session/session.h"
#include "event/storage_event.h"
#include "event/sql_event.h"
#include "event/session_event.h"
#include "sql/stmt/stmt.h"
#include "sql/stmt/select_stmt.h"
#include "storage/default/default_handler.h"
#include "sql/executor/command_executor.h"
#include "sql/operator/calc_physical_operator.h"
#include "sql/operator/trans_physical_operator.h"

using namespace std;
using namespace common;

RC ExecuteStage::handle_request(SQLStageEvent *sql_event)
{
  RC rc = RC::SUCCESS;
  const unique_ptr<PhysicalOperator> &physical_operator = sql_event->physical_operator();
  if (physical_operator != nullptr && sql_event->stmt()->type() != StmtType::CREATE_TABLE_SELECT) {
    return handle_request_with_physical_operator(sql_event);
  }

  SessionEvent *session_event = sql_event->session_event();

  Stmt *stmt = sql_event->stmt();
  if (stmt != nullptr) {
    CommandExecutor command_executor;
    rc = command_executor.execute(sql_event);
    session_event->sql_result()->set_return_code(rc);
  } else {
    return RC::INTERNAL;
  }
  return rc;
}

RC ExecuteStage::handle_request_with_physical_operator(SQLStageEvent *sql_event)
{
  RC rc = RC::SUCCESS;

  Stmt *stmt = sql_event->stmt();
  ASSERT(stmt != nullptr, "SQL Statement shouldn't be empty!");

  unique_ptr<PhysicalOperator> &physical_operator = sql_event->physical_operator();
  ASSERT(physical_operator != nullptr, "physical operator should not be null");

  // TODO 这里也可以优化一下，是否可以让physical operator自己设置tuple schema
  TupleSchema schema;
  switch (stmt->type()) {
    case StmtType::SELECT: {
      SelectStmt *select_stmt = static_cast<SelectStmt *>(stmt);
      bool with_table_name = select_stmt->tables().size() > 1;

      int num = 0;
      int group_by_begin = select_stmt->group_by_begin();
      int order_by_begin = select_stmt->order_by_begin();

      std::unordered_map<std::string, std::string> table_alias = select_stmt->table_alias();
      std::unordered_map<std::string, std::string> table_alias_reverse;
      for (auto pair : table_alias)
        table_alias_reverse[pair.second] = pair.first;
      std::vector<std::string> column_alias = select_stmt->column_alias();
      std::reverse(column_alias.begin(), column_alias.end());

      TransformPhysicalOperator *trans_oper = static_cast<TransformPhysicalOperator *>(physical_operator.get());

      std::unordered_map<int, std::string> exp_map;
      int exp_pos = 0;
      for (unique_ptr<Expression> &expression : trans_oper->expressions()) {
        if (expression == nullptr) {
          exp_pos ++;
          continue;
        }
        if (expression->type() == ExprType::FUNCTION || expression->type() == ExprType::ARITHMETIC) {
          exp_map.insert(std::pair<int, std::string>(exp_pos, expression->name()));
        }
        exp_pos ++;
      }

      for (const Field &field : select_stmt->query_fields()) {
        if (group_by_begin > -1 && num >= group_by_begin || order_by_begin > -1 && num >= order_by_begin)
          break;

        //expr
        auto exp_iter = exp_map.find(num);
        if (exp_iter != exp_map.end()) {
          schema.append_cell(exp_iter->second.c_str());
          num ++;
          continue;
        }
        //列有别名就直接用
        if (num < column_alias.size()) {
          std::string &alias = column_alias[num];
          if (alias != "") {
            schema.append_cell(alias.c_str());
            num ++;
            continue;
          }
        }    

        num ++;
        AggregationFunc func = field.func();
        if (with_table_name) {
          std::string table_name = table_alias_reverse[field.table_name()];
          if (func == NONE)
            schema.append_cell(table_name.c_str(), field.field_name());
          else{
            const char *field_name = field.is_star() ? "*" : field.field_name();
            schema.append_cell(TupleCellSpec(table_name.c_str(), (func_to_string(func) + "(" + field_name + ")").data(), 
              (func_to_string(func) + "(" + table_name + "." + field_name + ")").data()));
          }
        } else {
          if (func == NONE)
            schema.append_cell(field.field_name());
          else{
            const char *field_name = field.is_star() ? "*" : field.field_name();
            schema.append_cell((func_to_string(func) + "(" + field_name + ")").data());
          }
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

  SqlResult *sql_result = sql_event->session_event()->sql_result();
  sql_result->set_tuple_schema(schema);
  sql_result->set_operator(std::move(physical_operator));
  return rc;
}
