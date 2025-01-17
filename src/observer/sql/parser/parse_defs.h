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
// Created by Meiyi
//

#pragma once

#include <stddef.h>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

#include "sql/parser/value.h"

class Expression;

/**
 * @brief 表示聚合查询的类型
 * @ingroup SQLParser
 */
enum AggregationFunc
{
  NONE = 0,
  MAXFUN,
  MINFUN,
  COUNTFUN,
  AVGFUN,
  SUMFUN,
};

/**
 * @defgroup SQLParser SQL Parser 
 */

/**
 * @brief 描述一个属性
 * @ingroup SQLParser
 * @details 属性，或者说字段(column, field)
 * Rel -> Relation
 * Attr -> Attribute
 */
struct RelAttrSqlNode
{
  std::string relation_name;   ///< relation name (may be NULL) 表名
  std::string attribute_name;  ///< attribute name              属性名
  AggregationFunc func = NONE; ///< func                        聚合函数名
  std::string column_alias;///< column alias 列别名
  bool operator == (const RelAttrSqlNode& other) const {
    return relation_name == other.relation_name && attribute_name == other.attribute_name && func == other.func;
  }
  Expression *expr;
 
};

struct RelAttrSqlNode_hash_name {
  size_t operator()(const RelAttrSqlNode& r) const {
    size_t seed = 0;
    seed ^= std::hash<std::string>{}(r.relation_name) + 0x9e37a9bd + (seed << 8) + (seed >> 3);
    seed ^= std::hash<std::string>{}(r.attribute_name) + 0x903719bd + (seed << 6) + (seed >> 2);
    seed ^= std::hash<int>{}(r.func) + 0xa03819bd + (seed << 5) + (seed >> 4);
    return seed;
  }
};



/**
 * @brief 描述比较运算符
 * @ingroup SQLParser
 */
enum CompOp 
{
  EQUAL_TO,     ///< "="
  NOT_EQUAL,    ///< "<>" 
  LESS_EQUAL,    ///< "<="
  LESS_THAN,    ///< "<"
  GREAT_EQUAL,  ///< ">="
  GREAT_THAN,   ///< ">"
  LIKE_A,       ///< like
  LIKE_NOT,     ///< NOT LIKE 
  IN_OP,        ///< "IN"
  NOT_IN_OP,    ///< "NOT IN"
  EXISTS_OP,
  NOT_EXISTS_OP,
  NO_OP
};


/**
 * @brief 表示一个条件比较
 * @ingroup SQLParser
 * @details 条件比较就是SQL查询中的 where a>b 这种。
 * 一个条件比较是有两部分组成的，称为左边和右边。
 * 左边和右边理论上都可以是任意的数据，比如是字段（属性，列），也可以是数值常量。
 * 这个结构中记录的仅仅支持字段和值。
 */
struct SelectSqlNode;

/**
 * is_attr == 0 -> value
 * is_attr == 1 -> attr
 * is_attr == 2 -> select_stmt
 * is_attr == 3 -> values
 * is_attr == 4 -> expression
*/
struct ConditionSqlNode
{
  int             left_is_attr;    ///< TRUE if left-hand side is an attribute
                                   ///< 1时，操作符左边是属性名，0时，是属性值
  Value           left_value;      ///< left-hand side value if left_is_attr = FALSE
  RelAttrSqlNode  left_attr;       ///< left-hand side attribute
  Expression *    left_expr;
  CompOp          comp;            ///< comparison operator
  int             right_is_attr;   ///< TRUE if right-hand side is an attribute
                                   ///< 1时，操作符右边是属性名，0时，是属性值
  RelAttrSqlNode  right_attr;      ///< right-hand side attribute if right_is_attr = TRUE 右边的属性
  Value           right_value;     ///< right-hand side value if right_is_attr = FALSE
  SelectSqlNode   *right_select;    ///< right-hand side select stmt if right_is_attr = 2
  std::vector<Value> values;        /// 
  Expression *     right_expr;
};

/**
 * @brief 描述一个select语句
 * @ingroup SQLParser
 * @details 一个正常的select语句描述起来比这个要复杂很多，这里做了简化。
 * 一个select语句由三部分组成，分别是select, from, where。
 * select部分表示要查询的字段，from部分表示要查询的表，where部分表示查询的条件。
 * 比如 from 中可以是多个表，也可以是另一个查询语句，这里仅仅支持表，也就是 relations。
 * where 条件 conditions，这里表示使用AND串联起来多个条件。正常的SQL语句会有OR，NOT等，
 * 甚至可以包含复杂的表达式。
 */

struct SelectSqlNode
{
  std::vector<RelAttrSqlNode>     attributes;    ///< attributes in select clause
  std::vector<std::string>        relations;     ///< 查询的表
  std::vector<ConditionSqlNode>   conditions;    ///< 查询条件，使用AND串联起来多个条件
  std::vector<std::string>        group_by_columns;///< 分组依据的列名
  std::vector<ConditionSqlNode>   having_conditions;///< having子句
  std::vector<std::pair<RelAttrSqlNode, bool>> order_columns;///< 排序
  std::unordered_map<std::string, std::string> table_alias;///< 表别名
  std::vector<std::string> column_alias;///< 列别名
};


/**
 * @brief 算术表达式计算的语法树
 * @ingroup SQLParser
 */
struct CalcSqlNode
{
  std::vector<Expression *> expressions;  ///< calc clause

  ~CalcSqlNode();
};

/**
 * @brief 描述一个insert语句
 * @ingroup SQLParser
 * @details 于Selects类似，也做了很多简化
 */
struct InsertSqlNode
{
  std::string        relation_name;  ///< Relation to insert into
  std::vector<Value> values;         ///< 要插入的值
};

/**
 * @brief 描述一个delete语句
 * @ingroup SQLParser
 */
struct DeleteSqlNode
{
  std::string                   relation_name;  ///< Relation to delete from
  std::vector<ConditionSqlNode> conditions;
};

/**
 * @brief 描述一个update语句
 * @ingroup SQLParser
 */
struct AttrValuePair
{
  std::string                   attribute_name;
  Value                         value;
  void *                        ptr;
};

struct UpdateSqlNode
{
  std::string                   relation_name;         ///< Relation to update
  std::vector<AttrValuePair>    attr_value_pairs;        ///< 更新的字段，仅支持一个字段 
  std::vector<ConditionSqlNode> conditions;
  
};



/**
 * @brief 描述一个属性
 * @ingroup SQLParser
 * @details 属性，或者说字段(column, field)
 * Rel -> Relation
 * Attr -> Attribute
 */
struct AttrInfoSqlNode
{
  AttrType    type;       ///< Type of attribute
  std::string name;       ///< Attribute name
  size_t      length;     ///< Length of attribute
  bool nullable = false;  ///< nullable
};

/**
 * @brief 描述一个create table语句
 * @ingroup SQLParser
 * @details 这里也做了很多简化。
 */
struct CreateTableSqlNode
{
  std::string                  relation_name;         ///< Relation name
  std::vector<AttrInfoSqlNode> attr_infos;            ///< attributes
};

struct CreateTableSelectSqlNode
{
  std::string                 relation_name;
  void *                      select;            
};

/**
 * @brief 描述一个drop table语句
 * @ingroup SQLParser
 */
struct DropTableSqlNode
{
  std::string relation_name;  ///< 要删除的表名
};

/**
 * @brief 描述一个create index语句
 * @ingroup SQLParser
 * @details 创建索引时，需要指定索引名，表名，字段名。
 * 正常的SQL语句中，一个索引可能包含了多个字段，这里仅支持一个字段。
 */
struct CreateIndexSqlNode
{
  std::string index_name;      ///< Index name
  std::string relation_name;   ///< Relation name
  std::string attribute_name;  ///< Attribute name
};

struct CreateUniqueIndexSqlNode
{
  std::string index_name;
  std::string relation_name;
  std::vector<std::string> attribute_name;
};

/**
 * @brief create multi-index语句
 * @ingroup SQLParser
 * @details 创建multi索引，备选方案，直接返回success
 * TODO
*/
struct CreateMultiIndexSqlNode
{
  std::string index_name;       ///< Index name
  std::string relation_name;    ///< Relation name
  std::vector<std::string> attr_list;
};

/**
 * @brief 描述一个drop index语句
 * @ingroup SQLParser
 */
struct DropIndexSqlNode
{
  std::string index_name;     ///< Index name
  std::string relation_name;  ///< Relation name
};

/**
 * @brief 描述一个desc table语句
 * @ingroup SQLParser
 * @details desc table 是查询表结构信息的语句
 */
struct DescTableSqlNode
{
  std::string relation_name;
};

/**
 * @brief 描述一个load data语句
 * @ingroup SQLParser
 * @details 从文件导入数据到表中。文件中的每一行就是一条数据，每行的数据类型、字段个数都与表保持一致
 */
struct LoadDataSqlNode
{
  std::string relation_name;
  std::string file_name;
};

/**
 * @brief 设置变量的值
 * @ingroup SQLParser
 * @note 当前还没有查询变量
 */
struct SetVariableSqlNode
{
  std::string name;
  Value       value;
};

class ParsedSqlNode;

/**
 * @brief 描述一个explain语句
 * @ingroup SQLParser
 * @details 会创建operator的语句，才能用explain输出执行计划。
 * 一个command就是一个语句，比如select语句，insert语句等。
 * 可能改成SqlCommand更合适。
 */
struct ExplainSqlNode
{
  std::unique_ptr<ParsedSqlNode> sql_node;
};

/**
 * @brief 解析SQL语句出现了错误
 * @ingroup SQLParser
 * @details 当前解析时并没有处理错误的行号和列号
 */
struct ErrorSqlNode
{
  std::string error_msg;
  int         line;
  int         column;
};

/**
 * @brief 表示一个SQL语句的类型
 * @ingroup SQLParser
 */
enum SqlCommandFlag
{
  SCF_ERROR = 0,
  SCF_CALC,
  SCF_SELECT,
  SCF_INSERT,
  SCF_UPDATE,
  SCF_DELETE,
  SCF_CREATE_TABLE,
  SCF_CREATE_TABLE_SELECT,
  SCF_DROP_TABLE,
  SCF_CREATE_INDEX,
  SCF_CREATE_MULTIINDEX,
  SCF_CREATE_UNIQUE_INDEX,
  SCF_DROP_INDEX,
  SCF_SYNC,
  SCF_SHOW_TABLES,
  SCF_DESC_TABLE,
  SCF_BEGIN,        ///< 事务开始语句，可以在这里扩展只读事务
  SCF_COMMIT,
  SCF_CLOG_SYNC,
  SCF_ROLLBACK,
  SCF_LOAD_DATA,
  SCF_HELP,
  SCF_EXIT,
  SCF_EXPLAIN,
  SCF_SET_VARIABLE, ///< 设置变量
};
/**
 * @brief 表示一个SQL语句
 * @ingroup SQLParser
 */
class ParsedSqlNode
{
public:
  enum SqlCommandFlag       flag;
  ErrorSqlNode              error;
  CalcSqlNode               calc;
  SelectSqlNode             selection;
  InsertSqlNode             insertion;
  DeleteSqlNode             deletion;
  UpdateSqlNode             update;
  CreateTableSqlNode        create_table;
  CreateTableSelectSqlNode  create_table_select;
  DropTableSqlNode          drop_table;
  CreateIndexSqlNode        create_index;
  CreateMultiIndexSqlNode   create_multiIndex;
  CreateUniqueIndexSqlNode  create_unique_index;
  DropIndexSqlNode          drop_index;
  DescTableSqlNode          desc_table;
  LoadDataSqlNode           load_data;
  ExplainSqlNode            explain;
  SetVariableSqlNode        set_variable;

public:
  ParsedSqlNode();
  explicit ParsedSqlNode(SqlCommandFlag flag);
};

/**
 * @brief 表示语法解析后的数据
 * @ingroup SQLParser
 */
class ParsedSqlResult
{
public:
  void add_sql_node(std::unique_ptr<ParsedSqlNode> sql_node);
  std::vector<std::unique_ptr<ParsedSqlNode>> &sql_nodes()
  {
    return sql_nodes_;
  }

private:
  std::vector<std::unique_ptr<ParsedSqlNode>> sql_nodes_;  ///< 这里记录SQL命令。虽然看起来支持多个，但是当前仅处理一个
};
