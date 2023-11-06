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
  TransformLogicalOperator(std::vector<std::unique_ptr<Expression>> &expressions)
      
  {
    expressions_.swap(expressions);
    for (int i = 0; i < expressions_.size(); i++) {
      ArithmeticExpr &arithmetic = dynamic_cast<ArithmeticExpr &>(*expressions_[i]);
      FieldExpr &field = dynamic_cast<FieldExpr &>(*arithmetic.left());
      if (field.field().field_name() == nullptr) {
        std::cout << "FUCK" << std::endl;
      }
    }
  }
  virtual ~TransformLogicalOperator() = default;

  LogicalOperatorType type() const override { return LogicalOperatorType::TRANSFORM; }

};
