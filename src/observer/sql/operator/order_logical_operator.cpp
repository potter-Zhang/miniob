#include "sql/operator/order_logical_operator.h"

OrderLogicalOperator::OrderLogicalOperator(const std::vector<bool> &is_asc, int order_by_begin): is_asc_(is_asc), order_by_begin_(order_by_begin)
{}