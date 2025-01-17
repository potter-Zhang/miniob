
%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>

#include "common/log/log.h"
#include "common/lang/string.h"
#include "sql/parser/parse_defs.h"
#include "sql/parser/yacc_sql.hpp"
#include "sql/parser/lex_sql.h"
#include "sql/expr/expression.h"
#include "common/lang/date.h"

using namespace std;

string token_name(const char *sql_string, YYLTYPE *llocp)
{
  return string(sql_string + llocp->first_column, llocp->last_column - llocp->first_column + 1);
}

int yyerror(YYLTYPE *llocp, const char *sql_string, ParsedSqlResult *sql_result, yyscan_t scanner, const char *msg)
{
  std::unique_ptr<ParsedSqlNode> error_sql_node = std::make_unique<ParsedSqlNode>(SCF_ERROR);
  error_sql_node->error.error_msg = msg;
  error_sql_node->error.line = llocp->first_line;
  error_sql_node->error.column = llocp->first_column;
  sql_result->add_sql_node(std::move(error_sql_node));
  return 0;
}

ArithmeticExpr *create_arithmetic_expression(ArithmeticExpr::Type type,
                                             Expression *left,
                                             Expression *right,
                                             const char *sql_string,
                                             YYLTYPE *llocp)
{
  ArithmeticExpr *expr = new ArithmeticExpr(type, left, right);
  expr->set_name(token_name(sql_string, llocp));
  return expr;
}

ConditionSqlNode *always_false()
{
  ConditionSqlNode *result = new ConditionSqlNode;
  result->left_is_attr = 0;
  Value* vl = new Value(1);
  result->left_value = *vl;
  result->right_is_attr = 0;
  Value* vr = new Value(2);
  result->right_value = *vr;
  result->comp = CompOp::EQUAL_TO;
  return result;
}

FunctionExpr *create_function_expression(FunctionExpr::Type type, 
                                         Expression *expr,
                                         const char *sql_string,
                                         YYLTYPE *llocp)
{
  FunctionExpr *f_expr = new FunctionExpr(type, expr);
  f_expr->set_name(token_name(sql_string, llocp));
  return f_expr;
}

FunctionExpr *create_function_expression(FunctionExpr::Type type, 
                                         Expression *expr,
                                         const char *format,
                                         const char *sql_string,
                                         YYLTYPE *llocp)
{
  std::string format_string = std::string(format);
  FunctionExpr *f_expr = new FunctionExpr(type, expr, format_string);
  f_expr->set_name(token_name(sql_string, llocp));
  return f_expr;
}

FunctionExpr *create_function_expression(FunctionExpr::Type type, 
                                         Expression *expr,
                                         int round,
                                         const char *sql_string,
                                         YYLTYPE *llocp)
{
  FunctionExpr *f_expr = new FunctionExpr(type, expr, round);
  f_expr->set_name(token_name(sql_string, llocp));
  return f_expr;
}

void field_extract(Expression *expr, RelAttrSqlNode &rel_attr_node)
{
  if (expr->type() == ExprType::FIELD) {
    FieldExpr *f_expr = static_cast<FieldExpr *>(expr);
   
    rel_attr_node = f_expr->rel();
    rel_attr_node.column_alias = expr->name();
    //std::cout << rel_attr_node.attribute_name << std::endl;
    return;
  }
  if (expr->type() == ExprType::ARITHMETIC) {
    ArithmeticExpr *a_expr = static_cast<ArithmeticExpr *>(expr);
    field_extract(a_expr->left().get(), rel_attr_node);
    if (a_expr->arithmetic_type() != ArithmeticExpr::Type::NEGATIVE)
      field_extract(a_expr->right().get(), rel_attr_node);
    return;
  }
  if (expr->type() == ExprType::FUNCTION) {
    FunctionExpr *f_expr = static_cast<FunctionExpr *>(expr);
    field_extract(f_expr->expr().get(), rel_attr_node);
    return;
  }
}

%}

%define api.pure full
%define parse.error verbose
/** 启用位置标识 **/
%locations
%lex-param { yyscan_t scanner }
/** 这些定义了在yyparse函数中的参数 **/
%parse-param { const char * sql_string }
%parse-param { ParsedSqlResult * sql_result }
%parse-param { void * scanner }

//标识tokens
%token  SEMICOLON
        CREATE
        DROP
        TABLE
        TABLES
        INDEX
        CALC
        SELECT
        DESC
        SHOW
        SYNC
        INSERT
        DELETE
        UPDATE
        LBRACE
        RBRACE
        COMMA
        TRX_BEGIN
        TRX_COMMIT
        TRX_ROLLBACK
        INT_T
        STRING_T
        FLOAT_T
        DATE_T
        HELP
        EXIT
        DOT //QUOTE
        INTO
        VALUES
        FROM
        WHERE
        AND
        SET
        ON
        LOAD
        DATA
        INFILE
        EXPLAIN
        EQ
        LT
        GT
        LE
        GE
        NE
        LIKE
        NOT_LIKE
        MAX
        MIN
        COUNT
        AVG
        SUM
        GROUPBY
        INNERJOIN
        NOTNULL
        NULLABLE
        NULLVALUE
        ISNULL
        ISNOTNULL
        NULLISNULL
        NULLISNOTNULL
        HAVING
        IN
        NOT
        EXISTS
        ORDERBY
        ASC
        UNIQUE
        AS
        LENGTH
        ROUND
        DATE_FORMAT

/** union 中定义各种数据类型，真实生成的代码也是union类型，所以不能有非POD类型的数据 **/
%union {
  ParsedSqlNode *                   sql_node;
  ConditionSqlNode *                condition;
  Value *                           value;
  enum CompOp                       comp;
  RelAttrSqlNode *                  rel_attr;
  std::vector<AttrInfoSqlNode> *    attr_infos;
  AttrInfoSqlNode *                 attr_info;
  Expression *                      expression;
  std::vector<Expression *> *       expression_list;
  std::vector<Value> *              value_list;
  std::vector<Value> *              value_row;
  std::vector<Value> *              value_rows;
  std::vector<ConditionSqlNode> *   condition_list;
  std::vector<RelAttrSqlNode> *     rel_attr_list;
  std::vector<std::string> *        relation_list;
  std::vector<std::pair<RelAttrSqlNode, bool>>* unordered_list;
  std::pair<RelAttrSqlNode, bool>* pair;
  char *                            string;
  int                               number;
  float                             floats;
  char *                            dates;
  AggregationFunc                   func;
  std::vector<AttrValuePair> *      set_list;
}

%token <number> NUMBER
%token <floats> FLOAT
%token <string> DATE
%token <string> ID
%token <string> SSS
//非终结符

/** type 定义了各种解析后的结果输出的是什么类型。类型对应了 union 中的定义的成员变量名称 **/
%type <number>              type
%type <condition>           condition
%type <condition>           agg_condition
%type <value>               value
%type <number>              number
%type <comp>                comp_op
%type <rel_attr>            rel_attr
%type <rel_attr>            agg_attr
%type <attr_infos>          attr_def_list
%type <attr_info>           attr_def
%type <attr_info>           simple_attr_def
%type <value_list>          value_list
%type <value_row>           value_row
%type <value_rows>          value_rows
%type <condition_list>      where
%type <condition_list>      having
%type <condition_list>      condition_list
%type <condition_list>      agg_condition_list
%type <rel_attr_list>       select_attr
%type <relation_list>       rel_list
%type <relation_list>       tables
%type <relation_list>       group_by_columns
%type <string>              group_by_attr
%type <unordered_list>      unordered_list
%type <unordered_list>      order_list;
%type <pair>                order_attr
%type <relation_list>       group_by_attr_list
%type <rel_attr_list>       attr_list
%type <set_list>            set_list
%type <expression>          expression
%type <expression>          condition_expression
%type <expression_list>     expression_list
%type <expression_list>     part_expression_list
%type <sql_node>            calc_stmt
%type <sql_node>            select_stmt
%type <sql_node>            insert_stmt
%type <sql_node>            update_stmt
%type <sql_node>            delete_stmt
%type <sql_node>            create_table_stmt
%type <sql_node>            drop_table_stmt
%type <sql_node>            show_tables_stmt
%type <sql_node>            desc_table_stmt
%type <sql_node>            create_index_stmt
%type <sql_node>            create_multi_index_stmt
%type <sql_node>            drop_index_stmt
%type <sql_node>            sync_stmt
%type <sql_node>            begin_stmt
%type <sql_node>            commit_stmt
%type <sql_node>            rollback_stmt
%type <sql_node>            load_data_stmt
%type <sql_node>            explain_stmt
%type <sql_node>            set_variable_stmt
%type <sql_node>            help_stmt
%type <sql_node>            exit_stmt
%type <sql_node>            command_wrapper
%type <sql_node>            inner_join_node
%type <sql_node>            complete_inner_join_node
// commands should be a list but I use a single command instead
%type <sql_node>            commands
%type <func>                func

%left '+' '-'
%left '*' '/'
%nonassoc UMINUS
%%

commands: command_wrapper opt_semicolon  //commands or sqls. parser starts here.
  {
    std::unique_ptr<ParsedSqlNode> sql_node = std::unique_ptr<ParsedSqlNode>($1);
    sql_result->add_sql_node(std::move(sql_node));
  }
  ;

command_wrapper:
    calc_stmt
  | select_stmt
  | insert_stmt
  | update_stmt
  | delete_stmt
  | create_table_stmt
  | drop_table_stmt
  | show_tables_stmt
  | desc_table_stmt
  | create_index_stmt
  | create_multi_index_stmt
  | drop_index_stmt
  | sync_stmt
  | begin_stmt
  | commit_stmt
  | rollback_stmt
  | load_data_stmt
  | explain_stmt
  | set_variable_stmt
  | help_stmt
  | exit_stmt
    ;

exit_stmt:      
    EXIT {
      (void)yynerrs;  // 这么写为了消除yynerrs未使用的告警。如果你有更好的方法欢迎提PR
      $$ = new ParsedSqlNode(SCF_EXIT);
    };

help_stmt:
    HELP {
      $$ = new ParsedSqlNode(SCF_HELP);
    };

sync_stmt:
    SYNC {
      $$ = new ParsedSqlNode(SCF_SYNC);
    }
    ;

begin_stmt:
    TRX_BEGIN  {
      $$ = new ParsedSqlNode(SCF_BEGIN);
    }
    ;

commit_stmt:
    TRX_COMMIT {
      $$ = new ParsedSqlNode(SCF_COMMIT);
    }
    ;

rollback_stmt:
    TRX_ROLLBACK  {
      $$ = new ParsedSqlNode(SCF_ROLLBACK);
    }
    ;

drop_table_stmt:    /*drop table 语句的语法解析树*/
    DROP TABLE ID {
      $$ = new ParsedSqlNode(SCF_DROP_TABLE);
      $$->drop_table.relation_name = $3;
      free($3);
    };

show_tables_stmt:
    SHOW TABLES {
      $$ = new ParsedSqlNode(SCF_SHOW_TABLES);
    }
    ;

desc_table_stmt:
    DESC ID  {
      $$ = new ParsedSqlNode(SCF_DESC_TABLE);
      $$->desc_table.relation_name = $2;
      free($2);
    }
    ;

create_index_stmt:    /*create index 语句的语法解析树*/
    CREATE INDEX ID ON ID LBRACE ID RBRACE
    {
      $$ = new ParsedSqlNode(SCF_CREATE_INDEX);
      CreateIndexSqlNode &create_index = $$->create_index;
      create_index.index_name = $3;
      create_index.relation_name = $5;
      create_index.attribute_name = $7;
      free($3);
      free($5);
      free($7);
    }
    | CREATE UNIQUE INDEX ID ON ID LBRACE ID rel_list RBRACE
    {
      $$ = new ParsedSqlNode(SCF_CREATE_UNIQUE_INDEX);
      CreateUniqueIndexSqlNode &create_unique_index = $$->create_unique_index;
      create_unique_index.index_name = $4;
      create_unique_index.relation_name = $6;
      if ($9 != nullptr) {
        create_unique_index.attribute_name.swap(*$9);
      }
      create_unique_index.attribute_name.emplace_back($8);
      std::reverse(create_unique_index.attribute_name.begin(), create_unique_index.attribute_name.end());
      free($4);
      free($6);
      free($8);
    }
    ;

create_multi_index_stmt:
    CREATE INDEX ID ON ID LBRACE ID rel_list RBRACE
    {
      $$ = new ParsedSqlNode(SCF_CREATE_MULTIINDEX);
      CreateMultiIndexSqlNode &create_multiIndex = $$->create_multiIndex;
      create_multiIndex.index_name = $3;
      create_multiIndex.relation_name = $5;

      if($8 != nullptr) {
        create_multiIndex.attr_list.swap(*$8);
        delete $8;
      }
      create_multiIndex.attr_list.push_back($7);
      std::reverse(create_multiIndex.attr_list.begin(), create_multiIndex.attr_list.end());

      free($3);
      free($5);
      free($7);
    }
    ;

drop_index_stmt:      /*drop index 语句的语法解析树*/
    DROP INDEX ID ON ID
    {
      $$ = new ParsedSqlNode(SCF_DROP_INDEX);
      $$->drop_index.index_name = $3;
      $$->drop_index.relation_name = $5;
      free($3);
      free($5);
    }
    ;
create_table_stmt:    /*create table 语句的语法解析树*/
    CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE
    {
      $$ = new ParsedSqlNode(SCF_CREATE_TABLE);
      CreateTableSqlNode &create_table = $$->create_table;
      create_table.relation_name = $3;
      free($3);

      std::vector<AttrInfoSqlNode> *src_attrs = $6;

      if (src_attrs != nullptr) {
        create_table.attr_infos.swap(*src_attrs);
      }
      create_table.attr_infos.emplace_back(*$5);
      std::reverse(create_table.attr_infos.begin(), create_table.attr_infos.end());
      delete $5;
    }
    | CREATE TABLE ID AS select_stmt
    {
        $$ = new ParsedSqlNode(SCF_CREATE_TABLE_SELECT);
        CreateTableSelectSqlNode &create_table_select = $$->create_table_select;
        create_table_select.relation_name = $3;
        free($3);

        create_table_select.select = &$5->selection;
    }
    ;
attr_def_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | COMMA attr_def attr_def_list
    {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<AttrInfoSqlNode>;
      }
      $$->emplace_back(*$2);
      delete $2;
    }
    ;

attr_def:
    simple_attr_def
    {
      $$ = $1;
    }
    | simple_attr_def NOTNULL
    {
      $$ = $1;
    }
    | simple_attr_def NULLABLE
    {
      $$ = $1;
      $$->nullable = true;
      $$->length ++;
    }
    | simple_attr_def NULLVALUE
    {
      $$ = $1;
      $$->nullable = true;
      $$->length ++;
    }

simple_attr_def:
    ID type LBRACE number RBRACE 
    {
      $$ = new AttrInfoSqlNode;
      $$->type = (AttrType)$2;
      $$->name = $1;
      $$->length = $4;
      free($1);
    }
    | ID type
    {
      $$ = new AttrInfoSqlNode;
      $$->type = (AttrType)$2;
      $$->name = $1;
      $$->length = 4;
      free($1);
    }
    ;
number:
    NUMBER {$$ = $1;}
    ;
type:
    INT_T      { $$=INTS; }
    | STRING_T { $$=CHARS; }
    | FLOAT_T  { $$=FLOATS; }
    | DATE_T   { $$=DATES; }
    ;
insert_stmt:        /*insert   语句的语法解析树*/
    INSERT INTO ID VALUES value_row value_rows
    {
      $$ = new ParsedSqlNode(SCF_INSERT);
      $$->insertion.relation_name = $3;
      if ($6 != nullptr) {
        $$->insertion.values.swap(*$6);
      }
      std::copy($5->begin(), $5->end(), std::back_inserter($$->insertion.values));
      std::reverse($$->insertion.values.begin(), $$->insertion.values.end());
      delete $5;
      free($3);
    }
    ;

value_rows:
    /* empty */
    {
      $$ = nullptr;
    }
    | COMMA value_row value_rows{ 
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<Value>;
      }
      std::copy($2->begin(), $2->end(), std::back_inserter(*$$));
      delete $2;
    }
    ;

value_row:
    LBRACE value value_list RBRACE
    {
      $$ = new std::vector<Value>;
      if ($3 != nullptr) {
        $$->swap(*$3);
      }
      $$->emplace_back(*$2);
      delete $2;
    }
    ;

value_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | COMMA value value_list  { 
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<Value>;
      }
      $$->emplace_back(*$2);
      delete $2;
    }
    ;
value:
    NUMBER {
      $$ = new Value((int)$1);
      @$ = @1;
    }
    |FLOAT {
      $$ = new Value((float)$1);
      @$ = @1;
    }
    |DATE {
      char *tmp = common::substr($1,1,strlen($1)-2);
      $$ = new Value((Date)tmp);
      free(tmp);
      if (!$$->get_date().check()){
        YYABORT;
      }
    }
    |SSS {
      
        char *tmp = common::substr($1,1,strlen($1)-2);
        $$ = new Value(tmp);
        free(tmp);
      
    }
    |NULLVALUE{
      $$ = new Value();
      $$->set_type(AttrType::NULLTYPE);
      $$->set_nullable(true);
      $$->set_is_null(true);
    }

    ;
    
delete_stmt:    /*  delete 语句的语法解析树*/
    DELETE FROM ID where 
    {
      $$ = new ParsedSqlNode(SCF_DELETE);
      $$->deletion.relation_name = $3;
      if ($4 != nullptr) {
        $$->deletion.conditions.swap(*$4);
        delete $4;
      }
      free($3);
    }
    ;
update_stmt:      /*  update 语句的语法解析树*/
    UPDATE ID SET ID EQ value set_list where 
    {
      $$ = new ParsedSqlNode(SCF_UPDATE);
      if ($7 != nullptr) {
        $$->update.attr_value_pairs.swap(*$7);
      }
      $$->update.relation_name = $2;
      $$->update.attr_value_pairs.emplace_back(AttrValuePair{$4, *$6, nullptr});
      if ($8 != nullptr) {
        $$->update.conditions.swap(*$8);
        delete $8;
      }
      free($2);
      free($4);
    }
    | UPDATE ID SET ID EQ LBRACE select_stmt RBRACE set_list where
    {
      $$ = new ParsedSqlNode(SCF_UPDATE);
      if ($9 != nullptr) {
        $$->update.attr_value_pairs.swap(*$9);
      }
      $$->update.relation_name = $2;
      $$->update.attr_value_pairs.emplace_back(AttrValuePair{$4, Value(), &$7->selection});
     
      if ($10 != nullptr) {
        $$->update.conditions.swap(*$10);
        delete $10;
      }
      free($2);
      free($4);
    }
    ;
set_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | COMMA ID EQ value set_list
    {
      if ($5 != nullptr) {
        $$ = $5;
      } else {
        $$ = new std::vector<AttrValuePair>;
      }
      $$->emplace_back(AttrValuePair{$2, *$4, nullptr});
      delete $2;
      delete $4;
    }
    | COMMA ID EQ LBRACE select_stmt RBRACE set_list
    {
      if ($7 != nullptr) {
        $$ = $7;
      } else {
        $$ = new std::vector<AttrValuePair>;
      }
      $$->emplace_back(AttrValuePair{$2, Value(), &$5->selection});
      delete $2;
    }
select_stmt:        /*  select 语句的语法解析树*/
    SELECT select_attr FROM tables where group_by_columns having order_list
    {
      $$ = new ParsedSqlNode(SCF_SELECT);
      if ($2 != nullptr) {
        for (RelAttrSqlNode rel : *$2) {
          $$->selection.column_alias.emplace_back(rel.column_alias);
        }
        $$->selection.attributes.swap(*$2);
        delete $2;
      }
      for(std::string &table : *$4) {
        int pos;
        if ((pos = table.find(" ")) != table.npos) {
          std::string tmp = table;
          table = tmp.substr(0, pos);
          std::string alias = tmp.substr(pos + 1);
          $$->selection.table_alias.insert(std::pair<std::string, std::string>(alias, table));
          table = alias;
        }
      }
      $$->selection.relations.swap(*$4);
      delete $4;
      std::reverse($$->selection.relations.begin(), $$->selection.relations.end());

      if ($5 != nullptr) {
        $$->selection.conditions.swap(*$5);
        delete $5;
      }

      if($6 != nullptr) {
        $$->selection.group_by_columns.swap(*$6);
        std::reverse($$->selection.group_by_columns.begin(), $$->selection.group_by_columns.end());
        delete $6;
      }

      if($7 != nullptr) {
        $$->selection.having_conditions.swap(*$7);
        delete $7;
      }

      if($8 != nullptr) {
        $$->selection.order_columns.swap(*$8);
        delete $8;
      }
    }
    | SELECT select_attr FROM complete_inner_join_node where group_by_columns having order_list
    {
      if ($4 != nullptr){
        $$ = $4;
      } else {
        $$ = new ParsedSqlNode(SCF_SELECT);
      }
      std::reverse($$->selection.relations.begin(), $$->selection.relations.end());

      if ($2 != nullptr) {
        for (RelAttrSqlNode rel : *$2) {
          $$->selection.column_alias.emplace_back(rel.column_alias);
        }
        $$->selection.attributes.swap(*$2);
        delete $2;
      }

      if ($5 != nullptr) {
        std::copy($5->begin(), $5->end(), std::back_inserter($$->selection.conditions));
        delete $5;
      }

      if($6 != nullptr) {
        $$->selection.group_by_columns.swap(*$6);
        std::reverse($$->selection.group_by_columns.begin(), $$->selection.group_by_columns.end());
        delete $6;
      }

      if($7 != nullptr) {
        $$->selection.having_conditions.swap(*$7);
        delete $7;
      }

      if($8 != nullptr) {
        $$->selection.order_columns.swap(*$8);
        delete $8;
      }
    }
    ;
calc_stmt:
    CALC expression_list
    {
      $$ = new ParsedSqlNode(SCF_CALC);
      std::reverse($2->begin(), $2->end());
      $$->calc.expressions.swap(*$2);
      delete $2;
    }
    | SELECT expression_list
    {
      //std::cout << "calc" << std::endl;
      $$ = new ParsedSqlNode(SCF_CALC);
      std::reverse($2->begin(), $2->end());
      $$->calc.expressions.swap(*$2);
      delete $2;
    }
    ;

part_expression_list:    
     /* empty */
    {
      $$ = nullptr;
    }
    | COMMA expression part_expression_list
    {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<Expression *>;
      }

      $$->emplace_back($2);
    }
    | COMMA expression ID part_expression_list
    {
      if ($4 != nullptr) {
        $$ = $4;
      } else {
        $$ = new std::vector<Expression *>;
      }
      $2->set_name($3);
      $$->emplace_back($2);
    }
    | COMMA expression AS ID part_expression_list
    {
      if ($5 != nullptr) {
        $$ = $5;
      } else {
        $$ = new std::vector<Expression *>;
      }
      $2->set_name($4);
      $$->emplace_back($2);
    }
    ;

expression_list:
    expression part_expression_list
    {
      if ($2 != nullptr) {
        $$ = $2;
      } else {
        $$ = new std::vector<Expression*>;
      }

      $$->emplace_back($1);
    }
    | expression ID part_expression_list
    {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<Expression*>;
      }

      $1->set_name($2);
      $$->emplace_back($1);
    }
    | expression AS ID part_expression_list
    {
      if ($4 != nullptr) {
        $$ = $4;
      } else {
        $$ = new std::vector<Expression*>;
      }

      $1->set_name($3);
      $$->emplace_back($1);
    }
    ;
expression:
    expression '+' expression {
      $$ = create_arithmetic_expression(ArithmeticExpr::Type::ADD, $1, $3, sql_string, &@$);
    }
    | expression '-' expression {
      $$ = create_arithmetic_expression(ArithmeticExpr::Type::SUB, $1, $3, sql_string, &@$);
    }
    | expression '*' expression {
      $$ = create_arithmetic_expression(ArithmeticExpr::Type::MUL, $1, $3, sql_string, &@$);
    }
    | expression '/' expression {
      $$ = create_arithmetic_expression(ArithmeticExpr::Type::DIV, $1, $3, sql_string, &@$);
    }
    | LBRACE expression RBRACE {
      $$ = $2;
      $$->set_name(token_name(sql_string, &@$));
    }
    | LENGTH LBRACE expression RBRACE {
      $$ = create_function_expression(FunctionExpr::Type::LENGTH, $3, sql_string, &@$);
      //std::cout << "length" << std::endl;
    }
    | ROUND LBRACE expression RBRACE {
      $$ = create_function_expression(FunctionExpr::Type::ROUND, $3, 0, sql_string, &@$);
    }
    | ROUND LBRACE expression COMMA NUMBER RBRACE {
      $$ = create_function_expression(FunctionExpr::Type::ROUND, $3, $5, sql_string, &@$);
    }
    | DATE_FORMAT LBRACE expression COMMA SSS RBRACE {
      char *tmp = common::substr($5,1,strlen($5)-2);
      $$ = create_function_expression(FunctionExpr::Type::DATE_FORMAT, $3, tmp, sql_string, &@$);
      free(tmp);
    }
    | '-' expression %prec UMINUS {
      $$ = create_arithmetic_expression(ArithmeticExpr::Type::NEGATIVE, $2, nullptr, sql_string, &@$);
    }
    | expression NUMBER {
      Expression *num_expr = new ValueExpr(Value($2));
      $$ = create_arithmetic_expression(ArithmeticExpr::Type::ADD, $1, num_expr, sql_string, &@$);
    }
    | value {
      //std::cout << "value" << std::endl;
      $$ = new ValueExpr(*$1);
      $$->set_name(token_name(sql_string, &@$));
      delete $1;
    }
    | rel_attr {
      $$ = new FieldExpr(*$1);
      $$->set_name("");
      //$$->set_name(token_name(sql_string, &@$));
      delete $1;
    }
    ;

select_attr:
    expression_list
    {
      $$ = new std::vector<RelAttrSqlNode>;
      for (auto &expr : *$1) {
        RelAttrSqlNode rel_attr_node;
        //std::cout << "field ex" << std::endl;
        field_extract(expr, rel_attr_node);
        rel_attr_node.expr = expr;
        $$->emplace_back(rel_attr_node);
      }
     
      delete $1;
    }
    /*| rel_attr ID attr_list {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<RelAttrSqlNode>;
      }
      $1->column_alias = $2;
      $1->expr = nullptr;
      $$->emplace_back(*$1);
      
      delete $1;
      free($2);
    }
    | rel_attr AS ID attr_list {
      if ($4 != nullptr) {
        $$ = $4;
      } else {
        $$ = new std::vector<RelAttrSqlNode>;
      }
      $1->column_alias = $3;
      $1->expr = nullptr;
      $$->emplace_back(*$1);
      
      delete $1;
      free($3);
    }
    | rel_attr attr_list {
      if ($2 != nullptr) {
        $$ = $2;
      } else {
        $$ = new std::vector<RelAttrSqlNode>;
      }
      $1->expr = nullptr;
      $$->emplace_back(*$1);
      delete $1;
    }*/
    | '*' {
      $$ = new std::vector<RelAttrSqlNode>;
      RelAttrSqlNode attr;
      attr.relation_name  = "";
      attr.attribute_name = "*";
      attr.expr = nullptr;
      $$->emplace_back(attr);
      
    }
    ;

rel_attr:
    ID {
      $$ = new RelAttrSqlNode;
      $$->attribute_name = $1;
      free($1);
    }
    | ID DOT ID {
      $$ = new RelAttrSqlNode;
      $$->relation_name  = $1;
      $$->attribute_name = $3;
      free($1);
      free($3);
    }
    | ID DOT '*' {
      $$ = new RelAttrSqlNode;
      $$->relation_name  = $1;
      $$->attribute_name = '*';
      free($1);
    }
    | func LBRACE ID RBRACE {
      $$ = new RelAttrSqlNode;
      $$->attribute_name = $3;
      $$->func = $1;
      free($3);
    }
    | func LBRACE '*' RBRACE {
      $$ = new RelAttrSqlNode;
      $$->attribute_name = "*";
      if ($1 != AggregationFunc::COUNTFUN)
        YYABORT;
      $$->func = $1;
    }
    | func LBRACE ID DOT ID RBRACE {
      $$ = new RelAttrSqlNode;;
      $$->relation_name = $3;
      $$->attribute_name = $5;
      $$->func = $1;
      free($3);
      free($5);
    }
    ;

order_attr:
    rel_attr {
      $$ = new std::pair<RelAttrSqlNode, bool>;
      $$->first = *$1;
      $$->second = true;
      delete $1;
    }
    | rel_attr ASC {
      $$ = new std::pair<RelAttrSqlNode, bool>;
      $$->first = *$1;
      $$->second = true;
      delete $1;
    }
    | rel_attr DESC {
      $$ = new std::pair<RelAttrSqlNode, bool>;
      $$->first = *$1;
      $$->second = false;
      delete $1;
    }
    ;

agg_attr:
    func LBRACE ID RBRACE {
      $$ = new RelAttrSqlNode;
      $$->attribute_name = $3;
      $$->func = $1;
      free($3);
    }
    | func LBRACE '*' RBRACE {
      $$ = new RelAttrSqlNode;
      $$->attribute_name = "*";
      if ($1 != AggregationFunc::COUNTFUN)
        YYABORT;
      $$->func = $1;
    }
    | func LBRACE ID DOT ID RBRACE {
      $$ = new RelAttrSqlNode;;
      $$->relation_name = $3;
      $$->attribute_name = $5;
      $$->func = $1;
      free($3);
      free($5);
    }
    ;

attr_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | COMMA rel_attr attr_list {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<RelAttrSqlNode>;
      }

      $2->expr = nullptr;
      $$->emplace_back(*$2);
      delete $2;
    }
    | COMMA rel_attr ID attr_list {
      if ($4 != nullptr) {
        $$ = $4;
      } else {
        $$ = new std::vector<RelAttrSqlNode>;
      }

      $2->column_alias = $3;
      $2->expr = nullptr;
      $$->emplace_back(*$2);
      delete $2;
      free($3);
    }
    | COMMA rel_attr AS ID attr_list {
      if ($5 != nullptr) {
        $$ = $5;
      } else {
        $$ = new std::vector<RelAttrSqlNode>;
      }

      $2->column_alias = $4;
      $2->expr = nullptr;
      $$->emplace_back(*$2);
      delete $2;
      free($4);
    }
    ;

unordered_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | COMMA order_attr unordered_list
    {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<std::pair<RelAttrSqlNode, bool>>;
      }
      
      $$->emplace_back(*$2);
      delete $2;
    }
    ;

order_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | ORDERBY order_attr unordered_list
    {
      if ($3 == nullptr) {
        $$ = new std::vector<std::pair<RelAttrSqlNode, bool>>;
      } else {
        $$ = $3;
      }

      $$->emplace_back(*$2);
      delete $2;
    }
    ;

rel_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | COMMA ID rel_list {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<std::string>;
      }

      $$->push_back($2);
      free($2);
    }
    | COMMA ID ID rel_list {
      if ($4 != nullptr) {
        $$ = $4;
      } else {
        $$ = new std::vector<std::string>;
      }

      std::string name($2);
      name = name + " " + $3;
      $$->push_back(name);
      free($2);
      free($3);
    }
    | COMMA ID AS ID rel_list {
      if ($5 != nullptr) {
        $$ = $5;
      } else {
        $$ = new std::vector<std::string>;
      }

      std::string name($2);
      name = name + " " + $4;
      $$->push_back(name);
      free($2);
      free($4);
    }
    ;

tables:
    ID rel_list {
      if ($2 != nullptr) {
        $$ = $2;
      } else {
        $$ = new std::vector<std::string>;
      }
      $$->push_back($1);
      free($1);
    }
    | ID ID rel_list {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<std::string>;
      }
      std::string name($1);
      name = name + " " + $2;
      $$->push_back(name);
      free($1);
      free($2);
    }
    | ID AS ID rel_list {
      if ($4 != nullptr) {
        $$ = $4;
      } else {
        $$ = new std::vector<std::string>;
      }
      std::string name($1);
      name = name + " " + $3;
      $$->push_back(name);
      free($1);
      free($3);
    }
    ;

func:
    MAX {
      $$ = AggregationFunc::MAXFUN;
    }
    | MIN {
      $$ = AggregationFunc::MINFUN;
    }
    | COUNT {
      $$ = AggregationFunc::COUNTFUN;
    }
    | AVG {
      $$ = AggregationFunc::AVGFUN;
    }
    | SUM {
      $$ = AggregationFunc::SUMFUN;
    }
    ;
where:
    /* empty */
    {
      $$ = nullptr;
    }
    | WHERE condition_list {
      $$ = $2;  
    }
    ;
having:
    /* empty */
    {
      $$ = nullptr;
    }
    | HAVING agg_condition_list {
      $$ = $2;
    }
    ;
condition_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | condition {
      if ($1 != nullptr) {
        $$ = new std::vector<ConditionSqlNode>;
        $$->emplace_back(*$1);
        delete $1;
      }
      else
        $$ = nullptr;
    }
    | condition AND condition_list {
      $$ = $3;
      if ($1 != nullptr){
        if ($$ == nullptr)
          $$ = new std::vector<ConditionSqlNode>;
        $$->emplace_back(*$1);
        delete $1;
      }
    }
    ;

agg_condition_list:
    agg_condition {
      if ($1 != nullptr) {
        $$ = new std::vector<ConditionSqlNode>;
        $$->emplace_back(*$1);
        delete $1;
      }
      else
        $$ = nullptr;
    }
    | agg_condition AND agg_condition_list {
      $$ = $3;
      if ($1 != nullptr){
        if ($$ == nullptr)
          $$ = new std::vector<ConditionSqlNode>;
        $$->emplace_back(*$1);
        delete $1;
      }
    }
    ;

condition:
    /*rel_attr comp_op value
    {
      if ($3->nullable() && $3->is_null())
        $$ = always_false();
      else {
        $$ = new ConditionSqlNode;
        $$->left_is_attr = 1;
        $$->left_attr = *$1;
        $$->right_is_attr = 0;
        $$->right_value = *$3;
        $$->comp = $2;
      }

      delete $1;
      delete $3;
    }
    | value comp_op value 
    {
      if ($1->nullable() && $1->is_null())
        $$ = always_false();
      else if ($3->nullable() && $3->is_null())
        $$ = always_false();
      else {
        $$ = new ConditionSqlNode;
        $$->left_is_attr = 0;
        $$->left_value = *$1;
        $$->right_is_attr = 0;
        $$->right_value = *$3;
        $$->comp = $2;
      }

      delete $1;
      delete $3;
    }
    | rel_attr comp_op rel_attr
    {
      $$ = new ConditionSqlNode;
      $$->left_is_attr = 1;
      $$->left_attr = *$1;
      $$->right_is_attr = 1;
      $$->right_attr = *$3;
      $$->comp = $2;

      delete $1;
      delete $3;
    }
    | value comp_op rel_attr
    {
      if ($1->nullable() && $1->is_null())
        $$ = always_false();
      else {
        $$ = new ConditionSqlNode;
        $$->left_is_attr = 0;
        $$->left_value = *$1;
        $$->right_is_attr = 1;
        $$->right_attr = *$3;
        $$->comp = $2;
      }

      delete $1;
      delete $3;
    }
    |*/ value ISNULL
    {
      $$ = always_false();

      delete $1;
    }
    | rel_attr ISNULL
    {
      $$ = new ConditionSqlNode;
      $$->left_is_attr = 1;
      $$->left_attr = *$1;
      $$->right_is_attr = 0;
      Value null_value = Value();
      null_value.set_nullable(true);
      null_value.set_is_null(true);
      $$->right_value = null_value;
      $$->comp = CompOp::EQUAL_TO;

      delete $1;
    }
    | value ISNOTNULL
    {
      $$ = nullptr;

      delete $1;
    }
    | rel_attr ISNOTNULL
    {
      $$ = new ConditionSqlNode;
      $$->left_is_attr = 1;
      $$->left_attr = *$1;
      $$->right_is_attr = 0;
      Value null_value = Value();
      null_value.set_nullable(true);
      null_value.set_is_null(-1);
      $$->right_value = null_value;
      $$->comp = CompOp::EQUAL_TO;

      delete $1;
    }
    | NULLISNULL
    {
      $$ = nullptr;
    }
    | NULLISNOTNULL
    {
      $$ = always_false();
    }
    
    | rel_attr IN LBRACE select_stmt RBRACE
    {
      $$ = new ConditionSqlNode;
      $$->left_is_attr = 1;
      $$->left_attr = *$1;
      $$->right_is_attr = 2;
      $$->right_select = &($4->selection);
      $$->comp = CompOp::IN_OP;

      delete $1;
    }
    | value IN LBRACE select_stmt RBRACE
    {
      $$ = new ConditionSqlNode;
      $$->left_is_attr = 0;
      $$->left_value = *$1;
      $$->right_is_attr = 2;
      $$->right_select = &($4->selection);
      $$->comp = CompOp::IN_OP;

      delete $1;
    }
    | rel_attr NOT IN LBRACE select_stmt RBRACE
    {
      $$ = new ConditionSqlNode;
      $$->left_is_attr = 1;
      $$->left_attr = *$1;
      $$->right_is_attr = 2;
      $$->right_select = &($5->selection);
      $$->comp = CompOp::NOT_IN_OP;

      delete $1;
    }
    | value NOT IN LBRACE select_stmt RBRACE
    {
      $$ = new ConditionSqlNode;
      $$->left_is_attr = 0;
      $$->left_value = *$1;
      $$->right_is_attr = 2;
      $$->right_select = &($5->selection);
      $$->comp = CompOp::NOT_IN_OP;

      delete $1;
    }
    | rel_attr IN LBRACE value value_list RBRACE 
    {
      $$ = new ConditionSqlNode;
      if ($5 != nullptr) {
        $$->values.swap(*$5);
      }
      $$->values.emplace_back(*$4);
      std::reverse($$->values.begin(), $$->values.end());
      $$->left_is_attr = 1;
      $$->left_attr = *$1;
      $$->right_is_attr = 3;
      $$->comp = CompOp::IN_OP;
      delete $4;
      delete $1;
    }
    | value IN LBRACE value value_list RBRACE 
    {
      $$ = new ConditionSqlNode;
      if ($5 != nullptr) {
        $$->values.swap(*$5);
      }
      $$->values.emplace_back(*$4);
      std::reverse($$->values.begin(), $$->values.end());
      $$->left_is_attr = 0;
      $$->left_value = *$1;
      $$->right_is_attr = 3;
      $$->comp = CompOp::IN_OP;
      delete $4;
      delete $1;
    }
    | rel_attr NOT IN LBRACE value value_list RBRACE 
    {
      $$ = new ConditionSqlNode;
      if ($5 != nullptr) {
        $$->values.swap(*$6);
      }
      $$->values.emplace_back(*$5);
      std::reverse($$->values.begin(), $$->values.end());
      $$->left_is_attr = 1;
      $$->left_attr = *$1;
      $$->right_is_attr = 3;
      $$->comp = CompOp::NOT_IN_OP;
      delete $5;
      delete $1;
    }
    | value NOT IN LBRACE value value_list RBRACE 
    {
      $$ = new ConditionSqlNode;
      if ($5 != nullptr) {
        $$->values.swap(*$6);
      }
      $$->values.emplace_back(*$5);
      std::reverse($$->values.begin(), $$->values.end());
      $$->left_is_attr = 0;
      $$->left_value = *$1;
      $$->right_is_attr = 3;
      $$->comp = CompOp::NOT_IN_OP;
      delete $5;
      delete $1;
    }
    
    | EXISTS LBRACE select_stmt RBRACE
    {
      $$ = new ConditionSqlNode;
      $$->left_is_attr = 2;
      $$->right_is_attr = 2;
      $$->right_select = &($3->selection);
      $$->comp = CompOp::EXISTS_OP;

    }
    | NOT EXISTS LBRACE select_stmt RBRACE
    {
      $$ = new ConditionSqlNode;
      $$->left_is_attr = 2;
      $$->right_is_attr = 2;
      $$->right_select = &($4->selection);
      $$->comp = CompOp::NOT_EXISTS_OP;

    }
    
    | expression comp_op LBRACE select_stmt RBRACE
    {
      $$ = new ConditionSqlNode;
      $$->left_is_attr = 4;
      $$->left_expr = $1;
      $$->right_is_attr = 2;
      $$->right_select = &($4->selection);
      $$->comp = $2;

      
    }
    | LBRACE select_stmt RBRACE comp_op rel_attr 
    {
      $$ = new ConditionSqlNode;
      $$->left_is_attr = 1;
      $$->left_attr = *$5;
      $$->right_is_attr = 2;
      $$->right_select = &($2->selection);
      $$->comp = $4;
      if ($$->comp == CompOp::GREAT_EQUAL || $$->comp == CompOp::GREAT_THAN) {
        $$->comp = (CompOp)((int)$$->comp - 2);
      } else if ($$->comp == CompOp::LESS_EQUAL || $$->comp == CompOp::LESS_THAN) {
        $$->comp = (CompOp)((int)$$->comp + 2);
      }
      delete $5;
    }
    
    | expression comp_op expression
    {
      $$ = new ConditionSqlNode;
      $$->left_is_attr = 4;
      $$->left_expr = $1;
      $$->comp = $2;
      $$->right_is_attr = 4;
      $$->right_expr = $3;
    
    }
    ;

condition_expression:
    expression
    {
      $$ = $1;
    }
    | LBRACE select_stmt RBRACE
    {
      $$ = new new StmtExpr(*$2);
    }


agg_condition:
    agg_attr comp_op value
    {
      if ($3->nullable() && $3->is_null())
        $$ = always_false();
      else {
        $$ = new ConditionSqlNode;
        $$->left_is_attr = 1;
        $$->left_attr = *$1;
        $$->right_is_attr = 0;
        $$->right_value = *$3;
        $$->comp = $2;
      }

      delete $1;
      delete $3;
    }
    | value comp_op agg_attr
    {
      if ($1->nullable() && $1->is_null())
        $$ = always_false();
      else {
        $$ = new ConditionSqlNode;
        $$->left_is_attr = 0;
        $$->left_value = *$1;
        $$->right_is_attr = 1;
        $$->right_attr = *$3;
        $$->comp = $2;
      }

      delete $1;
      delete $3;
    }

comp_op:
      EQ { $$ = EQUAL_TO; }
    | LT { $$ = LESS_THAN; }
    | GT { $$ = GREAT_THAN; }
    | LE { $$ = LESS_EQUAL; }
    | GE { $$ = GREAT_EQUAL; }
    | NE { $$ = NOT_EQUAL; }
    | LIKE { $$ = LIKE_A; }
    | NOT_LIKE { $$ = LIKE_NOT; }
    ;

group_by_columns:
    /* empty */
    {
      $$ = nullptr;
    }
    | GROUPBY group_by_attr group_by_attr_list
    {
      $$ = new std::vector<std::string>;
      if ($3 != nullptr){
        $$->swap(*$3);
        delete $3;
      }
      $$->push_back($2);
      free($2);
    }

group_by_attr_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | COMMA group_by_attr group_by_attr_list{
      if ($3 != nullptr){
        $$ = $3;
      } else {
        $$ = new std::vector<std::string>;
      }

      $$->push_back($2);
      free($2);
    }
    ;

group_by_attr:
    ID {
      $$ = $1;
    }
    | ID DOT ID {
      int len1 = strlen($1);
      int len2 = strlen($3);
      int len = len1 + len2 + 2;
      char *temp = new char[len];
      temp[len1] = '.';
      temp[len - 1] = '\0';
      memcpy(temp, $1, len1);
      memcpy(temp + len1 + 1, $3, len2);
      $$ = temp;
    }
    ;

inner_join_node:
    /* empty */
    {
      $$ = nullptr;
    }
    | INNERJOIN ID inner_join_node
    {      
      if ($3 != nullptr){
        $$ = $3;
      } else {
        $$ = new ParsedSqlNode(SCF_SELECT);
      }

      $$->selection.relations.push_back($2);
      free($2);
    }
    | INNERJOIN ID ID inner_join_node
    {      
      if ($4 != nullptr){
        $$ = $4;
      } else {
        $$ = new ParsedSqlNode(SCF_SELECT);
      }

      $$->selection.relations.push_back($3);
      $$->selection.table_alias.insert(std::pair<std::string, std::string>($3, $2));
      free($2);
      free($3);
    }
    | INNERJOIN ID AS ID inner_join_node
    {      
      if ($5 != nullptr){
        $$ = $5;
      } else {
        $$ = new ParsedSqlNode(SCF_SELECT);
      }

      $$->selection.relations.push_back($4);
      $$->selection.table_alias.insert(std::pair<std::string, std::string>($4, $2));
      free($2);
      free($4);
    }
    | INNERJOIN ID ON condition_list inner_join_node
    {
      if ($5 != nullptr){
        $$ = $5;
      } else {
        $$ = new ParsedSqlNode(SCF_SELECT);
      }

      if ($4 == nullptr)
        YYABORT;
      std::copy($4->begin(), $4->end(), std::back_inserter($$->selection.conditions));
      $$->selection.relations.push_back($2);
      free($2);
    }
    | INNERJOIN ID ID ON condition_list inner_join_node
    {
      if ($6 != nullptr){
        $$ = $6;
      } else {
        $$ = new ParsedSqlNode(SCF_SELECT);
      }

      if ($5 == nullptr)
        YYABORT;
      std::copy($5->begin(), $5->end(), std::back_inserter($$->selection.conditions));
      $$->selection.relations.push_back($3);
      $$->selection.table_alias.insert(std::pair<std::string, std::string>($3, $2));
      free($2);
      free($3);
    }
    | INNERJOIN ID AS ID ON condition_list inner_join_node
    {
      if ($7 != nullptr){
        $$ = $7;
      } else {
        $$ = new ParsedSqlNode(SCF_SELECT);
      }

      if ($6 == nullptr)
        YYABORT;
      std::copy($6->begin(), $6->end(), std::back_inserter($$->selection.conditions));
      $$->selection.relations.push_back($4);
      $$->selection.table_alias.insert(std::pair<std::string, std::string>($4, $2));
      free($2);
      free($4);
    }
    ;

complete_inner_join_node:
    ID inner_join_node
    {
      if ($2 != nullptr) {
        $$ = $2;
      } else {
        $$ = new ParsedSqlNode(SCF_SELECT);
      }

      $$->selection.relations.push_back($1);
      free($1);
    }
    | ID ID inner_join_node
    {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new ParsedSqlNode(SCF_SELECT);
      }

      $$->selection.relations.push_back($2);
      $$->selection.table_alias.insert(std::pair<std::string, std::string>($2, $1));

      free($1);
      free($2);
    }
    | ID AS ID inner_join_node
    {
      if ($4 != nullptr) {
        $$ = $4;
      } else {
        $$ = new ParsedSqlNode(SCF_SELECT);
      }

      $$->selection.relations.push_back($3);
      $$->selection.table_alias.insert(std::pair<std::string, std::string>($3, $1));

      free($1);
      free($3);
    }
    ;

load_data_stmt:
    LOAD DATA INFILE SSS INTO TABLE ID 
    {
      char *tmp_file_name = common::substr($4, 1, strlen($4) - 2);
      
      $$ = new ParsedSqlNode(SCF_LOAD_DATA);
      $$->load_data.relation_name = $7;
      $$->load_data.file_name = tmp_file_name;
      free($7);
      free(tmp_file_name);
    }
    ;

explain_stmt:
    EXPLAIN command_wrapper
    {
      $$ = new ParsedSqlNode(SCF_EXPLAIN);
      $$->explain.sql_node = std::unique_ptr<ParsedSqlNode>($2);
    }
    ;

set_variable_stmt:
    SET ID EQ value
    {
      $$ = new ParsedSqlNode(SCF_SET_VARIABLE);
      $$->set_variable.name  = $2;
      $$->set_variable.value = *$4;
      free($2);
      delete $4;
    }
    ;

opt_semicolon: /*empty*/
    | SEMICOLON
    ;
%%
//_____________________________________________________________________
extern void scan_string(const char *str, yyscan_t scanner);

int sql_parse(const char *s, ParsedSqlResult *sql_result) {
  yyscan_t scanner;
  yylex_init(&scanner);
  scan_string(s, scanner);
  int result = yyparse(s, sql_result, scanner);
  yylex_destroy(scanner);
  return result;
}
