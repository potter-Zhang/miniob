/* Copyright (c) 2023 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2023/08/16.
//

#include "sql/optimizer/logical_plan_generator.h"

#include "sql/operator/logical_operator.h"
#include "sql/operator/calc_logical_operator.h"
#include "sql/operator/project_logical_operator.h"
#include "sql/operator/predicate_logical_operator.h"
#include "sql/operator/table_get_logical_operator.h"
#include "sql/operator/insert_logical_operator.h"
#include "sql/operator/delete_logical_operator.h"
#include "sql/operator/join_logical_operator.h"
#include "sql/operator/project_logical_operator.h"
#include "sql/operator/explain_logical_operator.h"
#include "sql/operator/update_logical_operator.h"
#include "sql/operator/group_logical_operator.h"
#include "sql/operator/aggregation_logical_operator.h"
#include "sql/operator/order_logical_operator.h"
#include "sql/operator/trans_logical_operator.h"

#include "sql/stmt/stmt.h"
#include "sql/stmt/calc_stmt.h"
#include "sql/stmt/select_stmt.h"
#include "sql/stmt/filter_stmt.h"
#include "sql/stmt/insert_stmt.h"
#include "sql/stmt/delete_stmt.h"
#include "sql/stmt/explain_stmt.h"
#include "sql/stmt/update_stmt.h"
#include "logical_plan_generator.h"
#include "sql/stmt/create_table_select_stmt.h"

using namespace std;

RC LogicalPlanGenerator::create(Stmt *stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  RC rc = RC::SUCCESS;
  switch (stmt->type()) {
    case StmtType::CALC: {
      CalcStmt *calc_stmt = static_cast<CalcStmt *>(stmt);
      rc = create_plan(calc_stmt, logical_operator);
    } break;

    case StmtType::CREATE_TABLE_SELECT: {
      CreateTableSelectStmt *create_table_select_stmt = static_cast<CreateTableSelectStmt *>(stmt);
      rc = create_plan(create_table_select_stmt->select_stmt(), logical_operator);
    } break;

    case StmtType::SELECT: {
      SelectStmt *select_stmt = static_cast<SelectStmt *>(stmt);
      rc = create_plan(select_stmt, logical_operator);
    } break;

    case StmtType::INSERT: {
      InsertStmt *insert_stmt = static_cast<InsertStmt *>(stmt);
      rc = create_plan(insert_stmt, logical_operator);
    } break;

    case StmtType::UPDATE: {
      UpdateStmt *update_stmt = static_cast<UpdateStmt *>(stmt);
      rc = create_plan(update_stmt, logical_operator);
    } break;

    case StmtType::DELETE: {
      DeleteStmt *delete_stmt = static_cast<DeleteStmt *>(stmt);
      rc = create_plan(delete_stmt, logical_operator);
    } break;

    case StmtType::EXPLAIN: {
      ExplainStmt *explain_stmt = static_cast<ExplainStmt *>(stmt);
      rc = create_plan(explain_stmt, logical_operator);
    } break;
    default: {
      rc = RC::UNIMPLENMENT;
    }
  }
  return rc;
}

RC LogicalPlanGenerator::create_plan(CalcStmt *calc_stmt, std::unique_ptr<LogicalOperator> &logical_operator)
{
  logical_operator.reset(new CalcLogicalOperator(std::move(calc_stmt->expressions())));
  return RC::SUCCESS;
}

RC LogicalPlanGenerator::create_plan(
    SelectStmt *select_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  unique_ptr<LogicalOperator> table_oper(nullptr);

  const std::vector<Table *> &tables = select_stmt->tables();
  const std::vector<Field> &all_fields = select_stmt->query_fields();
  int having_begin = select_stmt->having_begin();
  for (Table *table : tables) {
    std::vector<Field> fields;
    for (const Field &field : all_fields) {
      if (0 == strcmp(field.table_name(), table->name())) {
        fields.push_back(field);
      }
    }

    unique_ptr<LogicalOperator> table_get_oper(new TableGetLogicalOperator(table, fields, true/*readonly*/));
    if (table_oper == nullptr) {
      table_oper = std::move(table_get_oper);
    } else {
      JoinLogicalOperator *join_oper = new JoinLogicalOperator;
      join_oper->add_child(std::move(table_oper));
      join_oper->add_child(std::move(table_get_oper));
      table_oper = unique_ptr<LogicalOperator>(join_oper);
    }
  }

  unique_ptr<LogicalOperator> predicate_oper;
  RC rc = create_plan(select_stmt->filter_stmt(), predicate_oper, having_begin);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create predicate logical plan. rc=%s", strrc(rc));
    return rc;
  }


  unique_ptr<LogicalOperator> project_oper(new ProjectLogicalOperator(all_fields));
  unique_ptr<LogicalOperator> trans_oper(new TransformLogicalOperator(select_stmt->expressions()));
  if (predicate_oper) {
    if (table_oper) {
      predicate_oper->add_child(std::move(table_oper));
    }
    project_oper->add_child(std::move(predicate_oper));
  } else {
    if (table_oper) {
      project_oper->add_child(std::move(table_oper));
    }
  }

  unique_ptr<LogicalOperator> group_oper;
  int group_by_begin = select_stmt->group_by_begin();
  int attr_having_begin = select_stmt->attr_having_begin();
  std::unique_ptr<Expression> expression;
  rc = get_expressions_from_filter_stmt(select_stmt->filter_stmt(), expression, having_begin);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create having's expression. rc=%s", strrc(rc));
    return rc;
  }
  if (group_by_begin > -1){
    group_oper = unique_ptr<GroupLogicalOperator>(new GroupLogicalOperator(
      all_fields, std::move(expression), group_by_begin, attr_having_begin));
  }

  unique_ptr<LogicalOperator> agg_oper;
  bool group_exist = group_by_begin > -1;
  int cell_num = group_exist ? group_by_begin : all_fields.size();
  std::vector<AggregationFunc> funcs;
  bool func_exist = false;
  for (int i = 0; i < cell_num; i ++){
    AggregationFunc func = all_fields[i].func();
    if (func != NONE)
      func_exist = true;
    funcs.push_back(all_fields[i].func());
  }
  if (attr_having_begin > -1){
    for (int i = attr_having_begin; i < all_fields.size(); i ++)
      funcs.push_back(all_fields[i].func());
  }
  if (func_exist)
    agg_oper = unique_ptr<AggregationLogicalOperator>(new AggregationLogicalOperator(funcs, all_fields, group_exist));

  unique_ptr<LogicalOperator> oper;
  if (agg_oper)
    oper = std::move(agg_oper);
  if (group_exist){
    if (oper){
      group_oper->add_child(std::move(project_oper));
      oper->add_child(std::move(group_oper));
    }
    else{
      oper = std::move(group_oper);
      oper->add_child(std::move(project_oper));
    }
  }
  else if(oper){
    oper->add_child(std::move(project_oper));
  }
  if (!oper){
    oper = std::move(project_oper);
  }

  unique_ptr<OrderLogicalOperator> order_oper;
  int stmt_order_by_begin = select_stmt->order_by_begin();
  if (stmt_order_by_begin!= -1) {
    int order_by_begin;
    switch (oper->type())
    {
      case LogicalOperatorType::PROJECTION :
        order_by_begin = stmt_order_by_begin;
        break;
      case LogicalOperatorType::GROUP :
        order_by_begin = select_stmt->group_by_begin();
        break;
      case LogicalOperatorType::AGGREGATION :
        if (order_oper->children()[0]->type() == LogicalOperatorType::GROUP)
          order_by_begin = select_stmt->group_by_begin();
        else
          order_by_begin = stmt_order_by_begin;
        break;
      default :
        return RC::UNIMPLENMENT;
    }
    order_oper = unique_ptr<OrderLogicalOperator>(new OrderLogicalOperator(all_fields, select_stmt->is_asc(), order_by_begin));
    order_oper->add_child(std::move(oper));
    oper = std::move(order_oper);
  }

  trans_oper->add_child(std::move(oper));
  logical_operator.swap(trans_oper);
  return RC::SUCCESS;
}

RC LogicalPlanGenerator::create_plan(
    FilterStmt *filter_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  std::vector<unique_ptr<Expression>> cmp_exprs;
  std::vector<unique_ptr<LogicalOperator>> sub_queries;
  const std::vector<FilterUnit *> &filter_units = filter_stmt->filter_units();
  for (const FilterUnit *filter_unit : filter_units) {
    const FilterObj &filter_obj_left = filter_unit->left();
    const FilterObj &filter_obj_right = filter_unit->right();

    unique_ptr<Expression> left;
    if (filter_obj_left.is_attr == 1) {
      left.reset(static_cast<Expression *>(new FieldExpr(filter_obj_left.field)));
    } else if (filter_obj_left.is_attr == 0) {
      left.reset(static_cast<Expression *>(new ValueExpr(filter_obj_left.value)));
    } else if (filter_obj_left.is_attr == 4) {
      left.reset(static_cast<Expression *>(filter_obj_left.expr));
    }
    else {
      left.reset(static_cast<Expression *>(new EmptyExpr()));
    }

    unique_ptr<Expression> right;
    if (filter_obj_right.is_attr == 1) {
      right.reset(static_cast<Expression *>(new FieldExpr(filter_obj_right.field)));
    } else if (filter_obj_right.is_attr == 0) {
      right.reset(static_cast<Expression *>(new ValueExpr(filter_obj_right.value)));
    } else if (filter_obj_right.is_attr == 2) {
      unique_ptr<LogicalOperator> sub_logical_operator;
      create_plan(filter_obj_right.select_stmt_, sub_logical_operator);
      sub_queries.push_back(std::move(sub_logical_operator));
      right.reset(static_cast<Expression *>(new EmptyExpr()));
    } else if (filter_obj_right.is_attr == 4) {
      right.reset(static_cast<Expression *>(filter_obj_right.expr));
    }
    else {
      right.reset(static_cast<Expression *>(new CollectionExpr(filter_obj_right.values)));
    }
    
    ComparisonExpr *cmp_expr = new ComparisonExpr(filter_unit->comp(), std::move(left), std::move(right));
    cmp_exprs.emplace_back(cmp_expr);
  }

  unique_ptr<PredicateLogicalOperator> predicate_oper;
  if (!cmp_exprs.empty()) {
    unique_ptr<ConjunctionExpr> conjunction_expr(new ConjunctionExpr(ConjunctionExpr::Type::AND, cmp_exprs));
    predicate_oper = unique_ptr<PredicateLogicalOperator>(new PredicateLogicalOperator(std::move(conjunction_expr)));
  }

  if (!sub_queries.empty()) {
    predicate_oper->add_child(std::move(sub_queries[0]));
  }

  logical_operator = std::move(predicate_oper);

  

  return RC::SUCCESS;
}

RC LogicalPlanGenerator::create_plan(
    FilterStmt *filter_stmt, unique_ptr<LogicalOperator> &logical_operator, int limit)
{
  if (limit == -1)
    return create_plan(filter_stmt, logical_operator);
  std::vector<unique_ptr<Expression>> cmp_exprs;
  std::vector<unique_ptr<LogicalOperator>> sub_queries;
  const std::vector<FilterUnit *> &filter_units = filter_stmt->filter_units();
  int i = 0;
  for (const FilterUnit *filter_unit : filter_units) {
    i ++;
    if (i > limit)
      break;
    const FilterObj &filter_obj_left = filter_unit->left();
    const FilterObj &filter_obj_right = filter_unit->right();

    unique_ptr<Expression> left;
    if (filter_obj_left.is_attr == 1) {
      left.reset(static_cast<Expression *>(new FieldExpr(filter_obj_left.field)));
    } else if (filter_obj_right.is_attr == 0) {
      left.reset(static_cast<Expression *>(new ValueExpr(filter_obj_left.value)));
    } else {
      left.reset(static_cast<Expression *>(new EmptyExpr()));
    }

    unique_ptr<Expression> right;
    if (filter_obj_right.is_attr == 1) {
      right.reset(static_cast<Expression *>(new FieldExpr(filter_obj_right.field)));
    } else if (filter_obj_right.is_attr == 0) {
      right.reset(static_cast<Expression *>(new ValueExpr(filter_obj_right.value)));
    } else if (filter_obj_right.is_attr == 2) {
      unique_ptr<LogicalOperator> sub_logical_operator;
      create_plan(filter_obj_right.select_stmt_, sub_logical_operator);
      sub_queries.push_back(std::move(sub_logical_operator));
      right.reset(static_cast<Expression *>(new EmptyExpr()));
    } else {
      right.reset(static_cast<Expression *>(new CollectionExpr(filter_obj_right.values)));
    }
    
    ComparisonExpr *cmp_expr = new ComparisonExpr(filter_unit->comp(), std::move(left), std::move(right));
    cmp_exprs.emplace_back(cmp_expr);
  }

  unique_ptr<PredicateLogicalOperator> predicate_oper;
  if (!cmp_exprs.empty()) {
    unique_ptr<ConjunctionExpr> conjunction_expr(new ConjunctionExpr(ConjunctionExpr::Type::AND, cmp_exprs));
    predicate_oper = unique_ptr<PredicateLogicalOperator>(new PredicateLogicalOperator(std::move(conjunction_expr)));
  }

  if (!sub_queries.empty()) {
    predicate_oper->add_child(std::move(sub_queries[0]));
  }

  logical_operator = std::move(predicate_oper);

  

  return RC::SUCCESS;
}

RC LogicalPlanGenerator::create_plan(
    InsertStmt *insert_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  Table *table = insert_stmt->table();
  vector<Value> values(insert_stmt->values(), insert_stmt->values() + insert_stmt->value_amount());

  InsertLogicalOperator *insert_operator = new InsertLogicalOperator(table, values);
  logical_operator.reset(insert_operator);
  return RC::SUCCESS;
}

RC LogicalPlanGenerator::create_plan(
    DeleteStmt *delete_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  Table *table = delete_stmt->table();
  FilterStmt *filter_stmt = delete_stmt->filter_stmt();
  std::vector<Field> fields;
  for (int i = table->table_meta().sys_field_num(); i < table->table_meta().field_num(); i++) {
    const FieldMeta *field_meta = table->table_meta().field(i);
    fields.push_back(Field(table, field_meta));
  }
  unique_ptr<LogicalOperator> table_get_oper(new TableGetLogicalOperator(table, fields, false/*readonly*/));

  unique_ptr<LogicalOperator> predicate_oper;
  RC rc = create_plan(filter_stmt, predicate_oper);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  unique_ptr<LogicalOperator> delete_oper(new DeleteLogicalOperator(table));

  if (predicate_oper) {
    predicate_oper->add_child(std::move(table_get_oper));
    delete_oper->add_child(std::move(predicate_oper));
  } else {
    delete_oper->add_child(std::move(table_get_oper));
  }

  logical_operator = std::move(delete_oper);
  return rc;
}

RC LogicalPlanGenerator::create_plan(UpdateStmt *update_stmt, unique_ptr<LogicalOperator> &logical_operator) 
{
  Table *table = update_stmt->table();
  FilterStmt *filter_stmt = update_stmt->filter_stmt();
  std::vector<Field> fields;
  for (int i = table->table_meta().sys_field_num(); i < table->table_meta().field_num(); i++) {
    const FieldMeta *field_meta = table->table_meta().field(i);
    fields.push_back(Field(table, field_meta));
  }
  unique_ptr<LogicalOperator> table_get_oper(new TableGetLogicalOperator(table, fields, false/*readonly*/));

  unique_ptr<LogicalOperator> predicate_oper;
  RC rc = create_plan(filter_stmt, predicate_oper);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  unique_ptr<LogicalOperator> update_oper(new UpdateLogicalOperator(table, update_stmt->attr_value_pair()));
  

  if (predicate_oper) {
    predicate_oper->add_child(std::move(table_get_oper));
    update_oper->add_child(std::move(predicate_oper));
  } else {
    update_oper->add_child(std::move(table_get_oper));
  }
  std::vector<AttrValuePair> &attr_value_pairs = update_stmt->attr_value_pair();
  unique_ptr<LogicalOperator> select_oper;
  for (int i = 0; i < attr_value_pairs.size(); i++) {
    if (attr_value_pairs[i].ptr != nullptr) {
      rc = create_plan(static_cast<SelectStmt *>(attr_value_pairs[i].ptr), select_oper);
      update_oper->add_child(std::move(select_oper));
    }
  }
 
  logical_operator = std::move(update_oper);
  return rc;
}

RC LogicalPlanGenerator::get_expressions_from_filter_stmt(
    FilterStmt *filter_stmt, std::unique_ptr<Expression> &expression, int start)
{
  std::vector<unique_ptr<Expression>> cmp_exprs;
  const std::vector<FilterUnit *> &filter_units = filter_stmt->filter_units();
  if (start == -1)
    start = filter_units.size(); //让expression为空
  //for (const FilterUnit *filter_unit : filter_units) {
  for (int i = start; i < filter_units.size(); i ++) {
    const FilterObj &filter_obj_left = filter_units[i]->left();
    const FilterObj &filter_obj_right = filter_units[i]->right();

    unique_ptr<Expression> left(filter_obj_left.is_attr
                                         ? static_cast<Expression *>(new FieldExpr(filter_obj_left.field))
                                         : static_cast<Expression *>(new ValueExpr(filter_obj_left.value)));

    unique_ptr<Expression> right(filter_obj_right.is_attr
                                          ? static_cast<Expression *>(new FieldExpr(filter_obj_right.field))
                                          : static_cast<Expression *>(new ValueExpr(filter_obj_right.value)));

    ComparisonExpr *cmp_expr = new ComparisonExpr(filter_units[i]->comp(), std::move(left), std::move(right));
    cmp_exprs.emplace_back(cmp_expr);
  }

  expression = unique_ptr<ConjunctionExpr>(new ConjunctionExpr(ConjunctionExpr::Type::AND, cmp_exprs));
  return RC::SUCCESS;
}

RC LogicalPlanGenerator::create_plan(ExplainStmt *explain_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  Stmt *child_stmt = explain_stmt->child();
  unique_ptr<LogicalOperator> child_oper;
  RC rc = create(child_stmt, child_oper);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create explain's child operator. rc=%s", strrc(rc));
    return rc;
  }

  logical_operator = unique_ptr<LogicalOperator>(new ExplainLogicalOperator);
  logical_operator->add_child(std::move(child_oper));
  return rc;
}
