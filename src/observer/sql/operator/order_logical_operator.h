#pragma once

#include <vector>
#include <memory>

#include "sql/operator/logical_operator.h"
#include "sql/expr/expression.h"
#include "storage/field/field.h"

/**
 * @brief order 表示排序
 * @ingroup LogicalOperator
 * @details 在aggregation之上。
 */
class OrderLogicalOperator : public LogicalOperator 
{
public:
  OrderLogicalOperator(const std::vector<bool> &is_asc, int order_by_begin);
  virtual ~OrderLogicalOperator() = default;

  LogicalOperatorType type() const override
  {
    return LogicalOperatorType::ORDER;
  }

  std::vector<std::unique_ptr<Expression>> &expressions()
  {
    return expressions_;
  }
  const std::vector<std::unique_ptr<Expression>> &expressions() const
  {
    return expressions_;
  }
  const std::vector<bool> &is_asc() const
  {
    return is_asc_;
  }
  const bool order_by_begin() const
  {
    return order_by_begin_;
  }

private:
  std::vector<bool> is_asc_;

  int order_by_begin_ = -1;
};