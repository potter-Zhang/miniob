#pragma once

#include <memory>
#include <vector>

#include "sql/operator/logical_operator.h"

/**
 * @brief 逻辑算子描述当前执行计划要做什么
 * @details 可以看OptimizeStage中相关的代码
 */
class TransformLogicalOperator : public LogicalOperator
{
public:
  TransformLogicalOperator(std::vector<std::shared_ptr<Expression>> &expressions)
      
  {
    expressions_ = expressions;
  }
  virtual ~TransformLogicalOperator() = default;

  LogicalOperatorType type() const override { return LogicalOperatorType::TRANSFORM; }

};
