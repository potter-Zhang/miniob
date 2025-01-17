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
// Created by Wangyunlai on 2022/12/14.
//

#include <utility>

#include "sql/optimizer/physical_plan_generator.h"
#include "sql/operator/table_get_logical_operator.h"
#include "sql/operator/table_scan_physical_operator.h"
#include "sql/operator/index_scan_physical_operator.h"
#include "sql/operator/predicate_logical_operator.h"
#include "sql/operator/predicate_physical_operator.h"
#include "sql/operator/project_logical_operator.h"
#include "sql/operator/project_physical_operator.h"
#include "sql/operator/insert_logical_operator.h"
#include "sql/operator/insert_physical_operator.h"
#include "sql/operator/delete_logical_operator.h"
#include "sql/operator/delete_physical_operator.h"
#include "sql/operator/explain_logical_operator.h"
#include "sql/operator/explain_physical_operator.h"
#include "sql/operator/join_logical_operator.h"
#include "sql/operator/join_physical_operator.h"
#include "sql/operator/calc_logical_operator.h"
#include "sql/operator/calc_physical_operator.h"
#include "sql/operator/update_logical_operator.h"
#include "sql/operator/update_physical_operator.h"
#include "sql/operator/group_logical_operator.h"
#include "sql/operator/group_physical_operator.h"
#include "sql/operator/aggregation_logical_operator.h"
#include "sql/operator/aggregation_physical_operator.h"
#include "sql/operator/order_logical_operator.h"
#include "sql/operator/order_physical_operator.h"
#include "sql/operator/trans_physical_operator.h"
#include "sql/operator/trans_logical_operator.h"
#include "sql/expr/expression.h"
#include "common/log/log.h"
#include "physical_plan_generator.h"

using namespace std;

RC PhysicalPlanGenerator::create(LogicalOperator &logical_operator, unique_ptr<PhysicalOperator> &oper)
{
  RC rc = RC::SUCCESS;

  switch (logical_operator.type()) {
    case LogicalOperatorType::CALC: {
      return create_plan(static_cast<CalcLogicalOperator &>(logical_operator), oper);
    } break;

    case LogicalOperatorType::TABLE_GET: {
      return create_plan(static_cast<TableGetLogicalOperator &>(logical_operator), oper);
    } break;

    case LogicalOperatorType::PREDICATE: {
      return create_plan(static_cast<PredicateLogicalOperator &>(logical_operator), oper);
    } break;

    case LogicalOperatorType::PROJECTION: {
      return create_plan(static_cast<ProjectLogicalOperator &>(logical_operator), oper);
    } break;

    case LogicalOperatorType::INSERT: {
      return create_plan(static_cast<InsertLogicalOperator &>(logical_operator), oper);
    } break;

    case LogicalOperatorType::DELETE: {
      return create_plan(static_cast<DeleteLogicalOperator &>(logical_operator), oper);
    } break;

    case LogicalOperatorType::UPDATE: {
      return create_plan(static_cast<UpdateLogicalOperator &>(logical_operator), oper);
    } break;

    case LogicalOperatorType::GROUP: {
      return create_plan(static_cast<GroupLogicalOperator &>(logical_operator), oper);
    } break;

    case LogicalOperatorType::AGGREGATION: {
      return create_plan(static_cast<AggregationLogicalOperator &>(logical_operator), oper);
    } break;

    case LogicalOperatorType::ORDER: {
      return create_plan(static_cast<OrderLogicalOperator &>(logical_operator), oper);
    }

    case LogicalOperatorType::EXPLAIN: {
      return create_plan(static_cast<ExplainLogicalOperator &>(logical_operator), oper);
    } break;

    case LogicalOperatorType::JOIN: {
      return create_plan(static_cast<JoinLogicalOperator &>(logical_operator), oper);
    } break;

    case LogicalOperatorType::TRANSFORM: {
      return create_plan(static_cast<TransformLogicalOperator &>(logical_operator), oper);
    }

    default: {
      return RC::INVALID_ARGUMENT;
    }
  }
  return rc;
}



RC PhysicalPlanGenerator::create_plan(TableGetLogicalOperator &table_get_oper, unique_ptr<PhysicalOperator> &oper)
{
  vector<unique_ptr<Expression>> &predicates = table_get_oper.predicates();
  // 看看是否有可以用于索引查找的表达式
  Table *table = table_get_oper.table();

  Index *index = nullptr;
  ValueExpr *value_expr = nullptr;
  FieldExpr *field_expr = nullptr;
  for (auto &expr : predicates) {
    if (expr->type() == ExprType::COMPARISON) {
      auto comparison_expr = static_cast<ComparisonExpr *>(expr.get());
      // 简单处理，就找等值查询
      if (comparison_expr->comp() != EQUAL_TO) {
        continue;
      }

      unique_ptr<Expression> &left_expr = comparison_expr->left();
      unique_ptr<Expression> &right_expr = comparison_expr->right();
      // 左右比较的一边最少是一个值
      if (left_expr->type() != ExprType::VALUE && right_expr->type() != ExprType::VALUE) {
        continue;
      }

      field_expr = nullptr;
      if (left_expr->type() == ExprType::FIELD) {
        ASSERT(right_expr->type() == ExprType::VALUE, "right expr should be a value expr while left is field expr");
        field_expr = static_cast<FieldExpr *>(left_expr.get());
        value_expr = static_cast<ValueExpr *>(right_expr.get());
      } else if (right_expr->type() == ExprType::FIELD) {
        ASSERT(left_expr->type() == ExprType::VALUE, "left expr should be a value expr while right is a field expr");
        field_expr = static_cast<FieldExpr *>(right_expr.get());
        value_expr = static_cast<ValueExpr *>(left_expr.get());
      }

      if (field_expr == nullptr) {
        continue;
      }

      const Field &field = field_expr->field();
      index = table->find_index_by_field(field.field_name());
      if (nullptr != index) {
        if (field.attr_type() != value_expr->value_type() && !value_expr->try_convert_attr_type(field.attr_type())) {
          return RC::INVALID_ARGUMENT;
        }
        break;
      }

      
    }
  }

  if (index != nullptr) {
    ASSERT(value_expr != nullptr, "got an index but value expr is null ?");

    Value value = value_expr->get_value();
    if (field_expr->field().meta()->nullable()){
      value.set_nullable(true);
      value.get_data();
    }
    IndexScanPhysicalOperator *index_scan_oper = new IndexScanPhysicalOperator(
          table, index, table_get_oper.readonly(), 
          &value, true /*left_inclusive*/, 
          &value, true /*right_inclusive*/);
          
    index_scan_oper->set_predicates(std::move(predicates));
    oper = unique_ptr<PhysicalOperator>(index_scan_oper);
    LOG_TRACE("use index scan");
  } else {
    auto table_scan_oper = new TableScanPhysicalOperator(table, table_get_oper.readonly());
    table_scan_oper->set_predicates(std::move(predicates));
    oper = unique_ptr<PhysicalOperator>(table_scan_oper);
    LOG_TRACE("use table scan");
  }

  return RC::SUCCESS;
}

RC PhysicalPlanGenerator::create_plan(PredicateLogicalOperator &pred_oper, unique_ptr<PhysicalOperator> &oper)
{
  vector<unique_ptr<LogicalOperator>> &children_opers = pred_oper.children();
  ASSERT(children_opers.size() == 1, "predicate logical operator's sub oper number should be 1");

  LogicalOperator &child_oper = *children_opers.front();
  

  unique_ptr<PhysicalOperator> child_phy_oper;
  RC rc = create(child_oper, child_phy_oper);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create child operator of predicate operator. rc=%s", strrc(rc));
    return rc;
  }

  vector<unique_ptr<Expression>> &expressions = pred_oper.expressions();
  ASSERT(expressions.size() == 1, "predicate logical operator's children should be 1");

  unique_ptr<Expression> expression = std::move(expressions.front());
  oper = unique_ptr<PhysicalOperator>(new PredicatePhysicalOperator(std::move(expression)));
  if (children_opers.size() == 2) {
    LogicalOperator &brother_oper = *children_opers[1];
    unique_ptr<PhysicalOperator> brother_phy_oper;
    RC rc = create(brother_oper, brother_phy_oper);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    oper->add_child(std::move(brother_phy_oper));
  }
  oper->add_child(std::move(child_phy_oper));

  
  return rc;
}

RC PhysicalPlanGenerator::create_plan(ProjectLogicalOperator &project_oper, unique_ptr<PhysicalOperator> &oper)
{
  vector<unique_ptr<LogicalOperator>> &child_opers = project_oper.children();

  unique_ptr<PhysicalOperator> child_phy_oper;

  RC rc = RC::SUCCESS;
  if (!child_opers.empty()) {
    LogicalOperator *child_oper = child_opers.front().get();
    rc = create(*child_oper, child_phy_oper);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to create project logical operator's child physical operator. rc=%s", strrc(rc));
      return rc;
    }
  }

  //int group_by_begin = project_oper.group_by_begin();
  ProjectPhysicalOperator *project_operator = new ProjectPhysicalOperator();//group_by_begin);
  const vector<Field> &project_fields = project_oper.fields();
  for (const Field &field : project_fields) {
    project_operator->add_projection(field.table(), field.meta(), field.func());
  }
  /*if (project_operator->is_aggregation() && project_operator->group_by_begin() == -1) {
    std::vector<AggregationFunc> funcs = project_operator->funcs();
    if (std::find(funcs.begin(), funcs.end(), AggregationFunc::NONE) != funcs.end())
      return RC::INVALID_ARGUMENT;
  }*/ //resolve阶段就可以做检验了

  if (child_phy_oper) {
    project_operator->add_child(std::move(child_phy_oper));
  }

  oper = unique_ptr<PhysicalOperator>(project_operator);

  LOG_TRACE("create a project physical operator");
  return rc;
}

RC PhysicalPlanGenerator::create_plan(TransformLogicalOperator &trans_oper, unique_ptr<PhysicalOperator> &oper)
{
  vector<unique_ptr<LogicalOperator>> &children_opers = trans_oper.children();
  ASSERT(children_opers.size() == 1, "predicate logical operator's sub oper number should be 1");

  LogicalOperator &child_oper = *children_opers.front();
  
  unique_ptr<PhysicalOperator> child_phy_oper;
  RC rc = create(child_oper, child_phy_oper);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create child operator of predicate operator. rc=%s", strrc(rc));
    return rc;
  }

  vector<unique_ptr<Expression>> &expressions = trans_oper.expressions();

  oper = unique_ptr<PhysicalOperator>(new TransformPhysicalOperator(std::move(expressions)));
  
  if (child_phy_oper) {
    oper->add_child(std::move(child_phy_oper));
  }
  return rc;
}
  

RC PhysicalPlanGenerator::create_plan(InsertLogicalOperator &insert_oper, unique_ptr<PhysicalOperator> &oper)
{
  Table *table = insert_oper.table();
  vector<Value> &values = insert_oper.values();
  InsertPhysicalOperator *insert_phy_oper = new InsertPhysicalOperator(table, std::move(values));
  oper.reset(insert_phy_oper);
  return RC::SUCCESS;
}

RC PhysicalPlanGenerator::create_plan(DeleteLogicalOperator &delete_oper, unique_ptr<PhysicalOperator> &oper)
{
  vector<unique_ptr<LogicalOperator>> &child_opers = delete_oper.children();

  unique_ptr<PhysicalOperator> child_physical_oper;

  RC rc = RC::SUCCESS;
  if (!child_opers.empty()) {
    LogicalOperator *child_oper = child_opers.front().get();
    rc = create(*child_oper, child_physical_oper);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to create physical operator. rc=%s", strrc(rc));
      return rc;
    }
  }

  oper = unique_ptr<PhysicalOperator>(new DeletePhysicalOperator(delete_oper.table()));

  if (child_physical_oper) {
    oper->add_child(std::move(child_physical_oper));
  }
  return rc;
}

RC PhysicalPlanGenerator::create_plan(UpdateLogicalOperator &update_oper, unique_ptr<PhysicalOperator> &oper)
{
  vector<unique_ptr<LogicalOperator>> &child_opers = update_oper.children();

  unique_ptr<PhysicalOperator> child_physical_oper;
  unique_ptr<PhysicalOperator> select_physical_oper;
  
  Value &value = update_oper.value();

  RC rc = RC::SUCCESS;
  if (!child_opers.empty()) {
    LogicalOperator *child_oper = child_opers.front().get();
    rc = create(*child_oper, child_physical_oper);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to create physical operator. rc=%s", strrc(rc));
      return rc;
    }
  }

 
  oper = unique_ptr<PhysicalOperator>(new UpdatePhysicalOperator(update_oper.table(), update_oper.attr_value_pair()));

  if (child_physical_oper) {
    oper->add_child(std::move(child_physical_oper));
  }

  for (int i = 1; i < child_opers.size(); i++) {
    LogicalOperator *child_oper = child_opers[i].get();
    rc = create(*child_oper, select_physical_oper);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to create physical operator. rc=%s", strrc(rc));
      return rc;
    }
    oper->add_child(std::move(select_physical_oper));
  }
  
  return rc;

}

RC PhysicalPlanGenerator::create_plan(GroupLogicalOperator &group_oper, std::unique_ptr<PhysicalOperator> &oper)
{
  vector<unique_ptr<LogicalOperator>> &child_opers = group_oper.children();
  assert(child_opers.size() == 1);

  unique_ptr<PhysicalOperator> child_physical_oper;
  RC rc = create(*(child_opers[0]), child_physical_oper);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create child physical operator. rc=%s", strrc(rc));
    return rc;
  }

  const std::vector<Field>& fields = group_oper.fields();
  int group_by_begin = group_oper.group_by_begin();
  int attr_having_begin = group_oper.attr_having_begin();
  unique_ptr<Expression> &expression = group_oper.expressions()[0];

  unique_ptr<GroupPhysicalOperator> group_physical_oper = unique_ptr<GroupPhysicalOperator>(
    new GroupPhysicalOperator(std::move(expression), group_by_begin, attr_having_begin));
  for (Field field : fields)
    group_physical_oper->add_field(field);
  group_physical_oper->add_child(std::move(child_physical_oper));
  oper = std::move(group_physical_oper);
  return rc;
}

RC PhysicalPlanGenerator::create_plan(AggregationLogicalOperator &agg_oper, std::unique_ptr<PhysicalOperator> &oper)
{
  vector<unique_ptr<LogicalOperator>> &child_opers = agg_oper.children();
  assert(child_opers.size() == 1);

  unique_ptr<PhysicalOperator> child_physical_oper;
  RC rc = create(*(child_opers[0]), child_physical_oper);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create child physical operator. rc=%s", strrc(rc));
    return rc;
  }

  const std::vector<AggregationFunc>& funcs = agg_oper.funcs();
  const std::vector<Field>& fields = agg_oper.fields();
  bool group_by_exist = agg_oper.group_by_exist();
  unique_ptr<AggregationPhysicalOperator> agg_physical_oper = unique_ptr<AggregationPhysicalOperator>(new AggregationPhysicalOperator(group_by_exist));
  for (AggregationFunc func : funcs)
    agg_physical_oper->add_func(func);
  for (Field field : fields)
    agg_physical_oper->add_field(field);
  agg_physical_oper->add_child(std::move(child_physical_oper));
  oper = std::move(agg_physical_oper);
  return rc;
}

RC PhysicalPlanGenerator::create_plan(OrderLogicalOperator &order_oper, std::unique_ptr<PhysicalOperator> &oper)
{
  vector<unique_ptr<LogicalOperator>> &child_opers = order_oper.children();
  assert(child_opers.size() == 1);

  unique_ptr<PhysicalOperator> child_physical_oper;
  RC rc = create(*(child_opers[0]), child_physical_oper);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create child physical operator. rc=%s", strrc(rc));
    return rc;
  }

  const std::vector<bool> is_asc = order_oper.is_asc();
  int order_by_begin = order_oper.order_by_begin();
  const std::vector<Field>& fields = order_oper.fields();
  unique_ptr<OrderPhysicalOperator> order_physical_oper = unique_ptr<OrderPhysicalOperator>(new OrderPhysicalOperator(is_asc, order_by_begin));
  for (Field field : fields)
    order_physical_oper->add_field(field);
  order_physical_oper->add_child(std::move(child_physical_oper));
  oper = std::move(order_physical_oper);
  return rc;
}

RC PhysicalPlanGenerator::create_plan(ExplainLogicalOperator &explain_oper, unique_ptr<PhysicalOperator> &oper)
{
  vector<unique_ptr<LogicalOperator>> &child_opers = explain_oper.children();

  RC rc = RC::SUCCESS;
  unique_ptr<PhysicalOperator> explain_physical_oper(new ExplainPhysicalOperator);
  for (unique_ptr<LogicalOperator> &child_oper : child_opers) {
    unique_ptr<PhysicalOperator> child_physical_oper;
    rc = create(*child_oper, child_physical_oper);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to create child physical operator. rc=%s", strrc(rc));
      return rc;
    }

    explain_physical_oper->add_child(std::move(child_physical_oper));
  }

  oper = std::move(explain_physical_oper);
  return rc;
}

RC PhysicalPlanGenerator::create_plan(JoinLogicalOperator &join_oper, unique_ptr<PhysicalOperator> &oper)
{
  RC rc = RC::SUCCESS;

  vector<unique_ptr<LogicalOperator>> &child_opers = join_oper.children();
  if (child_opers.size() != 2) {
    LOG_WARN("join operator should have 2 children, but have %d", child_opers.size());
    return RC::INTERNAL;
  }

  unique_ptr<PhysicalOperator> join_physical_oper(new NestedLoopJoinPhysicalOperator);
  for (auto &child_oper : child_opers) {
    unique_ptr<PhysicalOperator> child_physical_oper;
    rc = create(*child_oper, child_physical_oper);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to create physical child oper. rc=%s", strrc(rc));
      return rc;
    }

    join_physical_oper->add_child(std::move(child_physical_oper));
  }

  oper = std::move(join_physical_oper);
  return rc;
}

RC PhysicalPlanGenerator::create_plan(CalcLogicalOperator &logical_oper, std::unique_ptr<PhysicalOperator> &oper)
{
  RC rc = RC::SUCCESS;
  CalcPhysicalOperator *calc_oper = new CalcPhysicalOperator(std::move(logical_oper.expressions()));
  oper.reset(calc_oper);
  return rc;
}

