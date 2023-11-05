/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 2 "yacc_sql.y"


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
    field_extract(f_expr->expr(), rel_attr_node);
    return;
  }
}


#line 186 "yacc_sql.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "yacc_sql.hpp"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_SEMICOLON = 3,                  /* SEMICOLON  */
  YYSYMBOL_CREATE = 4,                     /* CREATE  */
  YYSYMBOL_DROP = 5,                       /* DROP  */
  YYSYMBOL_TABLE = 6,                      /* TABLE  */
  YYSYMBOL_TABLES = 7,                     /* TABLES  */
  YYSYMBOL_INDEX = 8,                      /* INDEX  */
  YYSYMBOL_CALC = 9,                       /* CALC  */
  YYSYMBOL_SELECT = 10,                    /* SELECT  */
  YYSYMBOL_DESC = 11,                      /* DESC  */
  YYSYMBOL_SHOW = 12,                      /* SHOW  */
  YYSYMBOL_SYNC = 13,                      /* SYNC  */
  YYSYMBOL_INSERT = 14,                    /* INSERT  */
  YYSYMBOL_DELETE = 15,                    /* DELETE  */
  YYSYMBOL_UPDATE = 16,                    /* UPDATE  */
  YYSYMBOL_LBRACE = 17,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 18,                    /* RBRACE  */
  YYSYMBOL_COMMA = 19,                     /* COMMA  */
  YYSYMBOL_TRX_BEGIN = 20,                 /* TRX_BEGIN  */
  YYSYMBOL_TRX_COMMIT = 21,                /* TRX_COMMIT  */
  YYSYMBOL_TRX_ROLLBACK = 22,              /* TRX_ROLLBACK  */
  YYSYMBOL_INT_T = 23,                     /* INT_T  */
  YYSYMBOL_STRING_T = 24,                  /* STRING_T  */
  YYSYMBOL_FLOAT_T = 25,                   /* FLOAT_T  */
  YYSYMBOL_DATE_T = 26,                    /* DATE_T  */
  YYSYMBOL_HELP = 27,                      /* HELP  */
  YYSYMBOL_EXIT = 28,                      /* EXIT  */
  YYSYMBOL_DOT = 29,                       /* DOT  */
  YYSYMBOL_INTO = 30,                      /* INTO  */
  YYSYMBOL_VALUES = 31,                    /* VALUES  */
  YYSYMBOL_FROM = 32,                      /* FROM  */
  YYSYMBOL_WHERE = 33,                     /* WHERE  */
  YYSYMBOL_AND = 34,                       /* AND  */
  YYSYMBOL_SET = 35,                       /* SET  */
  YYSYMBOL_ON = 36,                        /* ON  */
  YYSYMBOL_LOAD = 37,                      /* LOAD  */
  YYSYMBOL_DATA = 38,                      /* DATA  */
  YYSYMBOL_INFILE = 39,                    /* INFILE  */
  YYSYMBOL_EXPLAIN = 40,                   /* EXPLAIN  */
  YYSYMBOL_EQ = 41,                        /* EQ  */
  YYSYMBOL_LT = 42,                        /* LT  */
  YYSYMBOL_GT = 43,                        /* GT  */
  YYSYMBOL_LE = 44,                        /* LE  */
  YYSYMBOL_GE = 45,                        /* GE  */
  YYSYMBOL_NE = 46,                        /* NE  */
  YYSYMBOL_LIKE = 47,                      /* LIKE  */
  YYSYMBOL_NOT_LIKE = 48,                  /* NOT_LIKE  */
  YYSYMBOL_MAX = 49,                       /* MAX  */
  YYSYMBOL_MIN = 50,                       /* MIN  */
  YYSYMBOL_COUNT = 51,                     /* COUNT  */
  YYSYMBOL_AVG = 52,                       /* AVG  */
  YYSYMBOL_SUM = 53,                       /* SUM  */
  YYSYMBOL_GROUPBY = 54,                   /* GROUPBY  */
  YYSYMBOL_INNERJOIN = 55,                 /* INNERJOIN  */
  YYSYMBOL_NOTNULL = 56,                   /* NOTNULL  */
  YYSYMBOL_NULLABLE = 57,                  /* NULLABLE  */
  YYSYMBOL_NULLVALUE = 58,                 /* NULLVALUE  */
  YYSYMBOL_ISNULL = 59,                    /* ISNULL  */
  YYSYMBOL_ISNOTNULL = 60,                 /* ISNOTNULL  */
  YYSYMBOL_NULLISNULL = 61,                /* NULLISNULL  */
  YYSYMBOL_NULLISNOTNULL = 62,             /* NULLISNOTNULL  */
  YYSYMBOL_HAVING = 63,                    /* HAVING  */
  YYSYMBOL_IN = 64,                        /* IN  */
  YYSYMBOL_NOT = 65,                       /* NOT  */
  YYSYMBOL_EXISTS = 66,                    /* EXISTS  */
  YYSYMBOL_ORDERBY = 67,                   /* ORDERBY  */
  YYSYMBOL_ASC = 68,                       /* ASC  */
  YYSYMBOL_UNIQUE = 69,                    /* UNIQUE  */
  YYSYMBOL_AS = 70,                        /* AS  */
  YYSYMBOL_LENGTH = 71,                    /* LENGTH  */
  YYSYMBOL_ROUND = 72,                     /* ROUND  */
  YYSYMBOL_DATE_FORMAT = 73,               /* DATE_FORMAT  */
  YYSYMBOL_NUMBER = 74,                    /* NUMBER  */
  YYSYMBOL_FLOAT = 75,                     /* FLOAT  */
  YYSYMBOL_DATE = 76,                      /* DATE  */
  YYSYMBOL_ID = 77,                        /* ID  */
  YYSYMBOL_SSS = 78,                       /* SSS  */
  YYSYMBOL_79_ = 79,                       /* '+'  */
  YYSYMBOL_80_ = 80,                       /* '-'  */
  YYSYMBOL_81_ = 81,                       /* '*'  */
  YYSYMBOL_82_ = 82,                       /* '/'  */
  YYSYMBOL_UMINUS = 83,                    /* UMINUS  */
  YYSYMBOL_YYACCEPT = 84,                  /* $accept  */
  YYSYMBOL_commands = 85,                  /* commands  */
  YYSYMBOL_command_wrapper = 86,           /* command_wrapper  */
  YYSYMBOL_exit_stmt = 87,                 /* exit_stmt  */
  YYSYMBOL_help_stmt = 88,                 /* help_stmt  */
  YYSYMBOL_sync_stmt = 89,                 /* sync_stmt  */
  YYSYMBOL_begin_stmt = 90,                /* begin_stmt  */
  YYSYMBOL_commit_stmt = 91,               /* commit_stmt  */
  YYSYMBOL_rollback_stmt = 92,             /* rollback_stmt  */
  YYSYMBOL_drop_table_stmt = 93,           /* drop_table_stmt  */
  YYSYMBOL_show_tables_stmt = 94,          /* show_tables_stmt  */
  YYSYMBOL_desc_table_stmt = 95,           /* desc_table_stmt  */
  YYSYMBOL_create_index_stmt = 96,         /* create_index_stmt  */
  YYSYMBOL_create_multi_index_stmt = 97,   /* create_multi_index_stmt  */
  YYSYMBOL_drop_index_stmt = 98,           /* drop_index_stmt  */
  YYSYMBOL_create_table_stmt = 99,         /* create_table_stmt  */
  YYSYMBOL_attr_def_list = 100,            /* attr_def_list  */
  YYSYMBOL_attr_def = 101,                 /* attr_def  */
  YYSYMBOL_simple_attr_def = 102,          /* simple_attr_def  */
  YYSYMBOL_number = 103,                   /* number  */
  YYSYMBOL_type = 104,                     /* type  */
  YYSYMBOL_insert_stmt = 105,              /* insert_stmt  */
  YYSYMBOL_value_rows = 106,               /* value_rows  */
  YYSYMBOL_value_row = 107,                /* value_row  */
  YYSYMBOL_value_list = 108,               /* value_list  */
  YYSYMBOL_value = 109,                    /* value  */
  YYSYMBOL_delete_stmt = 110,              /* delete_stmt  */
  YYSYMBOL_update_stmt = 111,              /* update_stmt  */
  YYSYMBOL_set_list = 112,                 /* set_list  */
  YYSYMBOL_select_stmt = 113,              /* select_stmt  */
  YYSYMBOL_calc_stmt = 114,                /* calc_stmt  */
  YYSYMBOL_part_expression_list = 115,     /* part_expression_list  */
  YYSYMBOL_expression_list = 116,          /* expression_list  */
  YYSYMBOL_expression = 117,               /* expression  */
  YYSYMBOL_select_attr = 118,              /* select_attr  */
  YYSYMBOL_rel_attr = 119,                 /* rel_attr  */
  YYSYMBOL_order_attr = 120,               /* order_attr  */
  YYSYMBOL_agg_attr = 121,                 /* agg_attr  */
  YYSYMBOL_unordered_list = 122,           /* unordered_list  */
  YYSYMBOL_order_list = 123,               /* order_list  */
  YYSYMBOL_rel_list = 124,                 /* rel_list  */
  YYSYMBOL_tables = 125,                   /* tables  */
  YYSYMBOL_func = 126,                     /* func  */
  YYSYMBOL_where = 127,                    /* where  */
  YYSYMBOL_having = 128,                   /* having  */
  YYSYMBOL_condition_list = 129,           /* condition_list  */
  YYSYMBOL_agg_condition_list = 130,       /* agg_condition_list  */
  YYSYMBOL_condition = 131,                /* condition  */
  YYSYMBOL_agg_condition = 132,            /* agg_condition  */
  YYSYMBOL_comp_op = 133,                  /* comp_op  */
  YYSYMBOL_group_by_columns = 134,         /* group_by_columns  */
  YYSYMBOL_group_by_attr_list = 135,       /* group_by_attr_list  */
  YYSYMBOL_group_by_attr = 136,            /* group_by_attr  */
  YYSYMBOL_inner_join_node = 137,          /* inner_join_node  */
  YYSYMBOL_complete_inner_join_node = 138, /* complete_inner_join_node  */
  YYSYMBOL_load_data_stmt = 139,           /* load_data_stmt  */
  YYSYMBOL_explain_stmt = 140,             /* explain_stmt  */
  YYSYMBOL_set_variable_stmt = 141,        /* set_variable_stmt  */
  YYSYMBOL_opt_semicolon = 142             /* opt_semicolon  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  79
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   497

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  84
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  59
/* YYNRULES -- Number of rules.  */
#define YYNRULES  181
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  377

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   334


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,    81,    79,     2,    80,     2,    82,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    83
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   301,   301,   309,   310,   311,   312,   313,   314,   315,
     316,   317,   318,   319,   320,   321,   322,   323,   324,   325,
     326,   327,   328,   329,   333,   339,   344,   350,   356,   362,
     368,   375,   381,   389,   400,   418,   439,   449,   465,   477,
     480,   493,   497,   501,   507,   515,   523,   533,   536,   537,
     538,   539,   542,   558,   561,   573,   586,   589,   600,   604,
     608,   616,   623,   633,   645,   660,   679,   682,   693,   704,
     749,   789,   796,   808,   811,   821,   831,   844,   854,   865,
     878,   881,   884,   887,   890,   894,   898,   901,   904,   909,
     912,   916,   922,   931,   980,   992,   997,  1004,  1010,  1016,
    1023,  1034,  1040,  1046,  1055,  1061,  1068,  1124,  1127,  1142,
    1145,  1160,  1163,  1173,  1186,  1202,  1211,  1223,  1238,  1241,
    1244,  1247,  1250,  1256,  1259,  1265,  1268,  1274,  1277,  1286,
    1298,  1307,  1381,  1387,  1401,  1407,  1421,  1425,  1430,  1441,
    1452,  1463,  1474,  1489,  1504,  1519,  1535,  1544,  1554,  1565,
    1581,  1605,  1621,  1639,  1640,  1641,  1642,  1643,  1644,  1645,
    1646,  1651,  1654,  1667,  1670,  1683,  1686,  1701,  1704,  1715,
    1728,  1741,  1755,  1771,  1790,  1801,  1815,  1832,  1845,  1853,
    1863,  1864
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "SEMICOLON", "CREATE",
  "DROP", "TABLE", "TABLES", "INDEX", "CALC", "SELECT", "DESC", "SHOW",
  "SYNC", "INSERT", "DELETE", "UPDATE", "LBRACE", "RBRACE", "COMMA",
  "TRX_BEGIN", "TRX_COMMIT", "TRX_ROLLBACK", "INT_T", "STRING_T",
  "FLOAT_T", "DATE_T", "HELP", "EXIT", "DOT", "INTO", "VALUES", "FROM",
  "WHERE", "AND", "SET", "ON", "LOAD", "DATA", "INFILE", "EXPLAIN", "EQ",
  "LT", "GT", "LE", "GE", "NE", "LIKE", "NOT_LIKE", "MAX", "MIN", "COUNT",
  "AVG", "SUM", "GROUPBY", "INNERJOIN", "NOTNULL", "NULLABLE", "NULLVALUE",
  "ISNULL", "ISNOTNULL", "NULLISNULL", "NULLISNOTNULL", "HAVING", "IN",
  "NOT", "EXISTS", "ORDERBY", "ASC", "UNIQUE", "AS", "LENGTH", "ROUND",
  "DATE_FORMAT", "NUMBER", "FLOAT", "DATE", "ID", "SSS", "'+'", "'-'",
  "'*'", "'/'", "UMINUS", "$accept", "commands", "command_wrapper",
  "exit_stmt", "help_stmt", "sync_stmt", "begin_stmt", "commit_stmt",
  "rollback_stmt", "drop_table_stmt", "show_tables_stmt",
  "desc_table_stmt", "create_index_stmt", "create_multi_index_stmt",
  "drop_index_stmt", "create_table_stmt", "attr_def_list", "attr_def",
  "simple_attr_def", "number", "type", "insert_stmt", "value_rows",
  "value_row", "value_list", "value", "delete_stmt", "update_stmt",
  "set_list", "select_stmt", "calc_stmt", "part_expression_list",
  "expression_list", "expression", "select_attr", "rel_attr", "order_attr",
  "agg_attr", "unordered_list", "order_list", "rel_list", "tables", "func",
  "where", "having", "condition_list", "agg_condition_list", "condition",
  "agg_condition", "comp_op", "group_by_columns", "group_by_attr_list",
  "group_by_attr", "inner_join_node", "complete_inner_join_node",
  "load_data_stmt", "explain_stmt", "set_variable_stmt", "opt_semicolon", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-296)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-94)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     394,     9,    34,   281,   206,   -58,    21,  -296,    20,     5,
     -28,  -296,  -296,  -296,  -296,  -296,   -12,    25,   394,    92,
     111,  -296,  -296,  -296,  -296,  -296,  -296,  -296,  -296,  -296,
    -296,  -296,  -296,  -296,  -296,  -296,  -296,  -296,  -296,  -296,
    -296,  -296,    46,    51,   127,    65,    66,   281,  -296,  -296,
    -296,  -296,  -296,  -296,   128,   129,   131,  -296,  -296,  -296,
     121,  -296,   281,  -296,  -296,   192,  -296,   134,  -296,   130,
     135,  -296,  -296,    76,    82,   126,   122,   142,  -296,  -296,
    -296,  -296,    19,   133,   105,  -296,   154,    98,   281,   281,
     281,   -48,   120,   281,   124,  -296,   180,   281,   281,   281,
     281,  -296,   -34,   132,   175,   174,   145,   110,   148,   151,
     200,   158,   197,   159,  -296,   123,    50,    -8,  -296,  -296,
     304,   180,  -296,   -47,   -47,   120,   120,    86,   234,     6,
     174,   174,   236,   244,  -296,   222,  -296,   235,   288,   248,
      63,   206,  -296,   251,   193,  -296,  -296,  -296,   202,   207,
     223,   180,  -296,  -296,  -296,   224,  -296,   226,   227,   230,
       2,  -296,  -296,   245,   245,   110,   306,   166,  -296,  -296,
     260,   310,   286,   346,   336,  -296,   294,   117,   323,  -296,
    -296,  -296,  -296,   318,   151,   319,  -296,  -296,  -296,  -296,
     259,   321,   329,   330,   180,  -296,   342,    11,    70,     2,
    -296,  -296,   293,   309,   309,   354,   236,  -296,   357,   359,
     200,  -296,  -296,   360,   315,  -296,  -296,  -296,  -296,  -296,
    -296,  -296,  -296,   291,  -296,  -296,   363,   333,   244,   200,
     383,   305,   337,   248,  -296,    81,   335,  -296,  -296,  -296,
    -296,   340,   399,  -296,   244,   347,   -14,  -296,  -296,  -296,
     384,   400,   386,   356,   356,   110,   412,   306,   408,   200,
     414,    35,   416,   166,   366,    35,   424,  -296,   425,   365,
     174,  -296,  -296,   439,  -296,  -296,   440,   399,   399,  -296,
     404,     3,   244,  -296,   388,   293,  -296,   408,   408,   446,
    -296,   432,   198,  -296,  -296,   354,  -296,  -296,   198,   449,
    -296,   354,   450,    35,   451,   354,   452,    35,   383,   430,
    -296,  -296,  -296,   454,  -296,  -296,   244,  -296,   404,  -296,
     400,   239,   110,    31,   386,     7,   455,  -296,  -296,  -296,
     457,  -296,   354,   458,  -296,   459,  -296,   354,   460,   174,
     156,  -296,   404,  -296,  -296,  -296,  -296,    89,   461,  -296,
    -296,  -296,   198,  -296,  -296,   462,  -296,  -296,   463,  -296,
    -296,   200,   383,  -296,  -296,   396,  -296,   455,  -296,  -296,
     464,  -296,   465,  -296,   383,  -296,  -296
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,    26,     0,     0,
       0,    27,    28,    29,    25,    24,     0,     0,     0,     0,
     180,    23,    22,    15,    16,    17,    18,     9,    10,    11,
      12,    13,    14,     8,     5,     7,     6,     4,     3,    19,
      20,    21,     0,     0,     0,     0,     0,     0,   118,   119,
     120,   121,   122,    62,     0,     0,     0,    58,    59,    60,
      95,    61,     0,    91,    71,    73,    92,     0,    94,    72,
       0,    32,    31,     0,     0,     0,     0,     0,   178,     1,
     181,     2,     0,     0,     0,    30,     0,     0,     0,     0,
       0,     0,    89,     0,     0,    90,    73,     0,     0,     0,
       0,    77,     0,     0,     0,   123,     0,     0,     0,     0,
       0,     0,     0,     0,    84,     0,     0,     0,    96,    97,
      73,    73,    78,    80,    81,    82,    83,     0,     0,   111,
     123,   123,     0,   127,    63,     0,   179,     0,     0,    39,
      41,     0,    38,     0,     0,    36,    85,    86,     0,     0,
       0,    73,    74,    79,    98,     0,    99,     0,     0,     0,
     111,   115,   174,   161,   161,     0,    53,     0,   136,   137,
       0,     0,    91,     0,    92,   124,   128,     0,     0,    48,
      49,    50,    51,    46,     0,     0,    42,    43,    44,    93,
       0,     0,     0,     0,    73,    75,     0,   111,   167,   111,
     116,   175,     0,   125,   125,    56,     0,    52,     0,     0,
       0,   132,   134,     0,     0,   153,   154,   155,   156,   157,
     158,   159,   160,     0,   133,   135,     0,     0,   127,     0,
      66,     0,     0,    39,    37,     0,     0,    87,    88,    76,
     100,     0,   111,   112,   127,     0,   167,   168,   117,   176,
     165,   163,     0,   109,   109,     0,     0,    53,     0,     0,
       0,     0,     0,     0,   150,     0,     0,   129,     0,     0,
     123,   177,    47,     0,    40,    33,     0,   111,   111,   113,
     167,   167,   127,   169,     0,     0,   162,     0,     0,     0,
     126,   130,     0,    69,    70,    56,    55,    54,     0,     0,
     146,    56,     0,     0,     0,    56,     0,     0,    66,     0,
      64,    45,    35,     0,   114,   171,   127,   170,   167,   166,
     163,     0,     0,     0,     0,   101,   107,    57,   149,   147,
       0,   139,    56,     0,   148,     0,   138,    56,     0,   123,
       0,    34,   167,   172,   164,   152,   151,     0,     0,   131,
     103,   102,     0,   110,   143,     0,   141,   142,     0,   140,
      65,     0,    66,   173,   104,     0,   105,   107,   145,   144,
       0,    67,     0,   108,    66,   106,    68
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -296,  -296,   466,  -296,  -296,  -296,  -296,  -296,  -296,  -296,
    -296,  -296,  -296,  -296,  -296,  -296,   252,   302,  -296,  -296,
    -296,  -296,   231,   283,  -281,   -95,  -296,  -296,  -295,  -107,
    -296,   -89,    -2,    -3,  -296,  -127,   138,   170,   125,   233,
    -151,  -296,  -244,  -126,   289,  -218,   171,  -296,  -296,  -235,
     332,   177,   209,  -144,  -296,  -296,  -296,  -296,  -296
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,   185,   139,   140,   273,
     183,    34,   207,   166,   256,    63,    35,    36,   270,    37,
      38,   101,    64,   173,    70,    66,   326,   288,   353,   293,
     161,   130,    67,   134,   253,   175,   290,   176,   291,   223,
     203,   286,   251,   162,   131,    39,    40,    41,    81
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      65,    65,    69,   142,   163,   164,   174,   122,   289,   200,
     267,   149,   136,   339,   327,    42,   201,    43,   350,    71,
     330,   157,   282,   298,   335,   157,   280,    95,    72,   118,
     157,   152,   153,   119,    99,   100,   109,    74,   172,   316,
      45,   158,    46,   127,    87,   141,   243,   128,   248,    75,
      73,   355,   321,   322,   247,   249,   358,   158,   158,    92,
     208,   158,   195,    77,   318,    76,    95,   371,   147,   148,
     205,    97,    98,    99,   100,   351,   159,   289,    44,   376,
     289,   241,   230,   160,   276,   115,   116,   117,   242,   110,
     120,   279,    79,    53,   123,   124,   125,   126,   342,   275,
     157,   174,   283,   260,   154,   239,   244,   364,   347,    57,
      58,    59,   348,    61,    80,   155,   114,   174,   365,   186,
     187,   188,   268,    82,    95,   158,   313,   314,    83,    97,
      98,    99,   100,   172,   229,    84,   315,   317,    65,   189,
     245,   146,    85,    86,   310,    88,    89,   246,    90,   172,
      91,   102,   299,   104,   302,   174,   304,   287,   306,   105,
     295,   106,   -93,   107,    87,   325,   301,   103,    53,   111,
     305,   328,    95,   361,   343,    53,   141,    97,    98,    99,
     100,   108,   112,    47,    57,    58,    59,   172,    61,   174,
     113,    57,    58,    59,    95,    61,   333,    95,   363,    93,
     338,   121,    97,    98,    99,   100,   132,   133,   332,   129,
     141,    93,   337,   360,    53,    48,    49,    50,    51,    52,
     264,   172,   135,    47,    53,   325,   137,   346,   138,   287,
      57,    58,    59,   144,    61,   143,   145,    54,    55,    56,
      57,    58,    59,    60,    61,   362,    62,    48,    49,    50,
      51,    52,   156,   165,   370,    48,    49,    50,    51,    52,
      87,   167,    94,   177,    53,   178,    95,   184,   190,    96,
     191,    97,    98,    99,   100,    60,   192,    54,    55,    56,
      57,    58,    59,    60,    61,   193,    62,    68,    48,    49,
      50,    51,    52,    48,    49,    50,    51,    52,    47,   202,
     194,   196,    53,   197,   198,   168,   169,   199,   263,   170,
     171,   179,   180,   181,   182,    54,    55,    56,    57,    58,
      59,    60,    61,    93,    62,   206,   209,   210,   228,   231,
      48,    49,    50,    51,    52,   232,   235,   234,   236,    53,
      48,    49,    50,    51,    52,   211,   212,   237,   238,    53,
     213,   214,    54,    55,    56,    57,    58,    59,    60,    61,
     240,    62,    54,    55,    56,    57,    58,    59,    60,    61,
     250,    62,   252,   255,   150,   258,   259,   261,    95,   262,
     265,   151,   271,    97,    98,    99,   100,   215,   216,   217,
     218,   219,   220,   221,   222,   224,   225,   266,     1,     2,
     226,   227,   269,     3,     4,     5,     6,     7,     8,     9,
      10,   272,   277,   284,    11,    12,    13,   278,   157,   285,
      95,    14,    15,   292,   281,    97,    98,    99,   100,    16,
     296,    17,   300,   303,    18,    48,    49,    50,    51,    52,
      95,   307,   309,   308,    53,    97,    98,    99,   100,   215,
     216,   217,   218,   219,   220,   221,   222,   311,   312,   158,
      57,    58,    59,   323,    61,   319,   324,   329,   331,   334,
     336,   340,   341,   372,   352,   354,   356,   357,   359,   366,
     368,   369,   374,   375,    78,   274,   233,   294,   297,   257,
     367,   345,   373,   254,   320,   349,   204,   344
};

static const yytype_int16 yycheck[] =
{
       3,     4,     4,   110,   130,   131,   133,    96,   252,   160,
     228,    19,   107,   308,   295,     6,   160,     8,    11,    77,
     301,    19,    36,   258,   305,    19,   244,    74,     7,    77,
      19,   120,   121,    81,    81,    82,    17,    32,   133,    36,
       6,    55,     8,    77,    47,    10,   197,    81,   199,    77,
      30,   332,   287,   288,   198,   199,   337,    55,    55,    62,
     167,    55,   151,    38,   282,    77,    74,   362,    18,    19,
     165,    79,    80,    81,    82,    68,    70,   321,    69,   374,
     324,    70,   177,    77,   235,    88,    89,    90,    77,    70,
      93,   242,     0,    58,    97,    98,    99,   100,   316,    18,
      19,   228,   246,   210,    18,   194,    36,    18,    77,    74,
      75,    76,    81,    78,     3,    29,    18,   244,    29,    56,
      57,    58,   229,    77,    74,    55,   277,   278,    77,    79,
      80,    81,    82,   228,    17,     8,   280,   281,   141,   141,
      70,    18,    77,    77,   270,    17,    17,    77,    17,   244,
      29,    17,   259,    77,   261,   282,   263,   252,   265,    77,
     255,    35,    32,    41,   167,   292,   261,    32,    58,    36,
     265,   298,    74,    17,   318,    58,    10,    79,    80,    81,
      82,    39,    77,    17,    74,    75,    76,   282,    78,   316,
      36,    74,    75,    76,    74,    78,   303,    74,   342,    19,
     307,    77,    79,    80,    81,    82,    31,    33,   303,    77,
      10,    19,   307,   339,    58,    49,    50,    51,    52,    53,
     223,   316,    77,    17,    58,   352,    78,   322,    77,   324,
      74,    75,    76,    36,    78,    77,    77,    71,    72,    73,
      74,    75,    76,    77,    78,   340,    80,    49,    50,    51,
      52,    53,    18,    17,   361,    49,    50,    51,    52,    53,
     263,    17,    70,    41,    58,    30,    74,    19,    17,    77,
      77,    79,    80,    81,    82,    77,    74,    71,    72,    73,
      74,    75,    76,    77,    78,    78,    80,    81,    49,    50,
      51,    52,    53,    49,    50,    51,    52,    53,    17,    54,
      77,    77,    58,    77,    77,    61,    62,    77,    17,    65,
      66,    23,    24,    25,    26,    71,    72,    73,    74,    75,
      76,    77,    78,    19,    80,    19,    66,    17,    34,     6,
      49,    50,    51,    52,    53,    17,    77,    18,    17,    58,
      49,    50,    51,    52,    53,    59,    60,    18,    18,    58,
      64,    65,    71,    72,    73,    74,    75,    76,    77,    78,
      18,    80,    71,    72,    73,    74,    75,    76,    77,    78,
      77,    80,    63,    19,    70,    18,    17,    17,    74,    64,
      17,    77,    77,    79,    80,    81,    82,    41,    42,    43,
      44,    45,    46,    47,    48,    59,    60,    64,     4,     5,
      64,    65,    19,     9,    10,    11,    12,    13,    14,    15,
      16,    74,    77,    29,    20,    21,    22,    77,    19,    19,
      74,    27,    28,    67,    77,    79,    80,    81,    82,    35,
      18,    37,    18,    17,    40,    49,    50,    51,    52,    53,
      74,    17,    77,    18,    58,    79,    80,    81,    82,    41,
      42,    43,    44,    45,    46,    47,    48,    18,    18,    55,
      74,    75,    76,    17,    78,    77,    34,    18,    18,    18,
      18,    41,    18,    77,    19,    18,    18,    18,    18,    18,
      18,    18,    18,    18,    18,   233,   184,   254,   257,   206,
     352,   321,   367,   204,   285,   324,   164,   320
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     4,     5,     9,    10,    11,    12,    13,    14,    15,
      16,    20,    21,    22,    27,    28,    35,    37,    40,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   105,   110,   111,   113,   114,   139,
     140,   141,     6,     8,    69,     6,     8,    17,    49,    50,
      51,    52,    53,    58,    71,    72,    73,    74,    75,    76,
      77,    78,    80,   109,   116,   117,   119,   126,    81,   116,
     118,    77,     7,    30,    32,    77,    77,    38,    86,     0,
       3,   142,    77,    77,     8,    77,    77,   117,    17,    17,
      17,    29,   117,    19,    70,    74,    77,    79,    80,    81,
      82,   115,    17,    32,    77,    77,    35,    41,    39,    17,
      70,    36,    77,    36,    18,   117,   117,   117,    77,    81,
     117,    77,   115,   117,   117,   117,   117,    77,    81,    77,
     125,   138,    31,    33,   127,    77,   109,    78,    77,   101,
     102,    10,   113,    77,    36,    77,    18,    18,    19,    19,
      70,    77,   115,   115,    18,    29,    18,    19,    55,    70,
      77,   124,   137,   127,   127,    17,   107,    17,    61,    62,
      65,    66,   109,   117,   119,   129,   131,    41,    30,    23,
      24,    25,    26,   104,    19,   100,    56,    57,    58,   116,
      17,    77,    74,    78,    77,   115,    77,    77,    77,    77,
     124,   137,    54,   134,   134,   109,    19,   106,   113,    66,
      17,    59,    60,    64,    65,    41,    42,    43,    44,    45,
      46,    47,    48,   133,    59,    60,    64,    65,    34,    17,
     109,     6,    17,   101,    18,    77,    17,    18,    18,   115,
      18,    70,    77,   124,    36,    70,    77,   137,   124,   137,
      77,   136,    63,   128,   128,    19,   108,   107,    18,    17,
     113,    17,    64,    17,   117,    17,    64,   129,   113,    19,
     112,    77,    74,   103,   100,    18,   124,    77,    77,   124,
     129,    77,    36,   137,    29,    19,   135,   109,   121,   126,
     130,   132,    67,   123,   123,   109,    18,   106,   133,   113,
      18,   109,   113,    17,   113,   109,   113,    17,    18,    77,
     127,    18,    18,   124,   124,   137,    36,   137,   129,    77,
     136,   133,   133,    17,    34,   119,   120,   108,   119,    18,
     108,    18,   109,   113,    18,   108,    18,   109,   113,   112,
      41,    18,   129,   137,   135,   121,   109,    77,    81,   130,
      11,    68,    19,   122,    18,   108,    18,    18,   108,    18,
     127,    17,   109,   137,    18,    29,    18,   120,    18,    18,
     113,   112,    77,   122,    18,    18,   112
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    84,    85,    86,    86,    86,    86,    86,    86,    86,
      86,    86,    86,    86,    86,    86,    86,    86,    86,    86,
      86,    86,    86,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    96,    97,    98,    99,    99,   100,
     100,   101,   101,   101,   101,   102,   102,   103,   104,   104,
     104,   104,   105,   106,   106,   107,   108,   108,   109,   109,
     109,   109,   109,   110,   111,   111,   112,   112,   112,   113,
     113,   114,   114,   115,   115,   115,   115,   116,   116,   116,
     117,   117,   117,   117,   117,   117,   117,   117,   117,   117,
     117,   117,   117,   118,   118,   119,   119,   119,   119,   119,
     119,   120,   120,   120,   121,   121,   121,   122,   122,   123,
     123,   124,   124,   124,   124,   125,   125,   125,   126,   126,
     126,   126,   126,   127,   127,   128,   128,   129,   129,   129,
     130,   130,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   132,   132,   133,   133,   133,   133,   133,   133,   133,
     133,   134,   134,   135,   135,   136,   136,   137,   137,   137,
     137,   137,   137,   137,   138,   138,   138,   139,   140,   141,
     142,   142
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     2,     2,     8,    10,     9,     5,     7,     5,     0,
       3,     1,     2,     2,     2,     5,     2,     1,     1,     1,
       1,     1,     6,     0,     3,     4,     0,     3,     1,     1,
       1,     1,     1,     4,     8,    10,     0,     5,     7,     8,
       8,     2,     2,     0,     3,     4,     5,     2,     3,     4,
       3,     3,     3,     3,     3,     4,     4,     6,     6,     2,
       2,     1,     1,     1,     1,     1,     3,     3,     4,     4,
       6,     1,     2,     2,     4,     4,     6,     0,     3,     0,
       3,     0,     3,     4,     5,     2,     3,     4,     1,     1,
       1,     1,     1,     0,     2,     0,     2,     0,     1,     3,
       1,     3,     2,     2,     2,     2,     1,     1,     5,     5,
       6,     6,     6,     6,     7,     7,     4,     5,     5,     5,
       3,     3,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     0,     3,     0,     3,     1,     3,     0,     3,     4,
       5,     5,     6,     7,     2,     3,     4,     7,     2,     4,
       0,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (&yylloc, sql_string, sql_result, scanner, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location, sql_string, sql_result, scanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  YY_USE (sql_string);
  YY_USE (sql_result);
  YY_USE (scanner);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp, sql_string, sql_result, scanner);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule, const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]), sql_string, sql_result, scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, sql_string, sql_result, scanner); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
  YYLTYPE *yylloc;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;
      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  YY_USE (sql_string);
  YY_USE (sql_result);
  YY_USE (scanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, &yylloc, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* commands: command_wrapper opt_semicolon  */
#line 302 "yacc_sql.y"
  {
    std::unique_ptr<ParsedSqlNode> sql_node = std::unique_ptr<ParsedSqlNode>((yyvsp[-1].sql_node));
    sql_result->add_sql_node(std::move(sql_node));
  }
#line 2009 "yacc_sql.cpp"
    break;

  case 24: /* exit_stmt: EXIT  */
#line 333 "yacc_sql.y"
         {
      (void)yynerrs;  // 这么写为了消除yynerrs未使用的告警。如果你有更好的方法欢迎提PR
      (yyval.sql_node) = new ParsedSqlNode(SCF_EXIT);
    }
#line 2018 "yacc_sql.cpp"
    break;

  case 25: /* help_stmt: HELP  */
#line 339 "yacc_sql.y"
         {
      (yyval.sql_node) = new ParsedSqlNode(SCF_HELP);
    }
#line 2026 "yacc_sql.cpp"
    break;

  case 26: /* sync_stmt: SYNC  */
#line 344 "yacc_sql.y"
         {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SYNC);
    }
#line 2034 "yacc_sql.cpp"
    break;

  case 27: /* begin_stmt: TRX_BEGIN  */
#line 350 "yacc_sql.y"
               {
      (yyval.sql_node) = new ParsedSqlNode(SCF_BEGIN);
    }
#line 2042 "yacc_sql.cpp"
    break;

  case 28: /* commit_stmt: TRX_COMMIT  */
#line 356 "yacc_sql.y"
               {
      (yyval.sql_node) = new ParsedSqlNode(SCF_COMMIT);
    }
#line 2050 "yacc_sql.cpp"
    break;

  case 29: /* rollback_stmt: TRX_ROLLBACK  */
#line 362 "yacc_sql.y"
                  {
      (yyval.sql_node) = new ParsedSqlNode(SCF_ROLLBACK);
    }
#line 2058 "yacc_sql.cpp"
    break;

  case 30: /* drop_table_stmt: DROP TABLE ID  */
#line 368 "yacc_sql.y"
                  {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DROP_TABLE);
      (yyval.sql_node)->drop_table.relation_name = (yyvsp[0].string);
      free((yyvsp[0].string));
    }
#line 2068 "yacc_sql.cpp"
    break;

  case 31: /* show_tables_stmt: SHOW TABLES  */
#line 375 "yacc_sql.y"
                {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SHOW_TABLES);
    }
#line 2076 "yacc_sql.cpp"
    break;

  case 32: /* desc_table_stmt: DESC ID  */
#line 381 "yacc_sql.y"
             {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DESC_TABLE);
      (yyval.sql_node)->desc_table.relation_name = (yyvsp[0].string);
      free((yyvsp[0].string));
    }
#line 2086 "yacc_sql.cpp"
    break;

  case 33: /* create_index_stmt: CREATE INDEX ID ON ID LBRACE ID RBRACE  */
#line 390 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_INDEX);
      CreateIndexSqlNode &create_index = (yyval.sql_node)->create_index;
      create_index.index_name = (yyvsp[-5].string);
      create_index.relation_name = (yyvsp[-3].string);
      create_index.attribute_name = (yyvsp[-1].string);
      free((yyvsp[-5].string));
      free((yyvsp[-3].string));
      free((yyvsp[-1].string));
    }
#line 2101 "yacc_sql.cpp"
    break;

  case 34: /* create_index_stmt: CREATE UNIQUE INDEX ID ON ID LBRACE ID rel_list RBRACE  */
#line 401 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_UNIQUE_INDEX);
      CreateUniqueIndexSqlNode &create_unique_index = (yyval.sql_node)->create_unique_index;
      create_unique_index.index_name = (yyvsp[-6].string);
      create_unique_index.relation_name = (yyvsp[-4].string);
      if ((yyvsp[-1].relation_list) != nullptr) {
        create_unique_index.attribute_name.swap(*(yyvsp[-1].relation_list));
      }
      create_unique_index.attribute_name.emplace_back((yyvsp[-2].string));
      std::reverse(create_unique_index.attribute_name.begin(), create_unique_index.attribute_name.end());
      free((yyvsp[-6].string));
      free((yyvsp[-4].string));
      free((yyvsp[-2].string));
    }
#line 2120 "yacc_sql.cpp"
    break;

  case 35: /* create_multi_index_stmt: CREATE INDEX ID ON ID LBRACE ID rel_list RBRACE  */
#line 419 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_MULTIINDEX);
      CreateMultiIndexSqlNode &create_multiIndex = (yyval.sql_node)->create_multiIndex;
      create_multiIndex.index_name = (yyvsp[-6].string);
      create_multiIndex.relation_name = (yyvsp[-4].string);

      if((yyvsp[-1].relation_list) != nullptr) {
        create_multiIndex.attr_list.swap(*(yyvsp[-1].relation_list));
        delete (yyvsp[-1].relation_list);
      }
      create_multiIndex.attr_list.push_back((yyvsp[-2].string));
      std::reverse(create_multiIndex.attr_list.begin(), create_multiIndex.attr_list.end());

      free((yyvsp[-6].string));
      free((yyvsp[-4].string));
      free((yyvsp[-2].string));
    }
#line 2142 "yacc_sql.cpp"
    break;

  case 36: /* drop_index_stmt: DROP INDEX ID ON ID  */
#line 440 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DROP_INDEX);
      (yyval.sql_node)->drop_index.index_name = (yyvsp[-2].string);
      (yyval.sql_node)->drop_index.relation_name = (yyvsp[0].string);
      free((yyvsp[-2].string));
      free((yyvsp[0].string));
    }
#line 2154 "yacc_sql.cpp"
    break;

  case 37: /* create_table_stmt: CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE  */
#line 450 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_TABLE);
      CreateTableSqlNode &create_table = (yyval.sql_node)->create_table;
      create_table.relation_name = (yyvsp[-4].string);
      free((yyvsp[-4].string));

      std::vector<AttrInfoSqlNode> *src_attrs = (yyvsp[-1].attr_infos);

      if (src_attrs != nullptr) {
        create_table.attr_infos.swap(*src_attrs);
      }
      create_table.attr_infos.emplace_back(*(yyvsp[-2].attr_info));
      std::reverse(create_table.attr_infos.begin(), create_table.attr_infos.end());
      delete (yyvsp[-2].attr_info);
    }
#line 2174 "yacc_sql.cpp"
    break;

  case 38: /* create_table_stmt: CREATE TABLE ID AS select_stmt  */
#line 466 "yacc_sql.y"
    {
        (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_TABLE_SELECT);
        CreateTableSelectSqlNode &create_table_select = (yyval.sql_node)->create_table_select;
        create_table_select.relation_name = (yyvsp[-2].string);
        free((yyvsp[-2].string));

        create_table_select.select = &(yyvsp[0].sql_node)->selection;
    }
#line 2187 "yacc_sql.cpp"
    break;

  case 39: /* attr_def_list: %empty  */
#line 477 "yacc_sql.y"
    {
      (yyval.attr_infos) = nullptr;
    }
#line 2195 "yacc_sql.cpp"
    break;

  case 40: /* attr_def_list: COMMA attr_def attr_def_list  */
#line 481 "yacc_sql.y"
    {
      if ((yyvsp[0].attr_infos) != nullptr) {
        (yyval.attr_infos) = (yyvsp[0].attr_infos);
      } else {
        (yyval.attr_infos) = new std::vector<AttrInfoSqlNode>;
      }
      (yyval.attr_infos)->emplace_back(*(yyvsp[-1].attr_info));
      delete (yyvsp[-1].attr_info);
    }
#line 2209 "yacc_sql.cpp"
    break;

  case 41: /* attr_def: simple_attr_def  */
#line 494 "yacc_sql.y"
    {
      (yyval.attr_info) = (yyvsp[0].attr_info);
    }
#line 2217 "yacc_sql.cpp"
    break;

  case 42: /* attr_def: simple_attr_def NOTNULL  */
#line 498 "yacc_sql.y"
    {
      (yyval.attr_info) = (yyvsp[-1].attr_info);
    }
#line 2225 "yacc_sql.cpp"
    break;

  case 43: /* attr_def: simple_attr_def NULLABLE  */
#line 502 "yacc_sql.y"
    {
      (yyval.attr_info) = (yyvsp[-1].attr_info);
      (yyval.attr_info)->nullable = true;
      (yyval.attr_info)->length ++;
    }
#line 2235 "yacc_sql.cpp"
    break;

  case 44: /* attr_def: simple_attr_def NULLVALUE  */
#line 508 "yacc_sql.y"
    {
      (yyval.attr_info) = (yyvsp[-1].attr_info);
      (yyval.attr_info)->nullable = true;
      (yyval.attr_info)->length ++;
    }
#line 2245 "yacc_sql.cpp"
    break;

  case 45: /* simple_attr_def: ID type LBRACE number RBRACE  */
#line 516 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-3].number);
      (yyval.attr_info)->name = (yyvsp[-4].string);
      (yyval.attr_info)->length = (yyvsp[-1].number);
      free((yyvsp[-4].string));
    }
#line 2257 "yacc_sql.cpp"
    break;

  case 46: /* simple_attr_def: ID type  */
#line 524 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[0].number);
      (yyval.attr_info)->name = (yyvsp[-1].string);
      (yyval.attr_info)->length = 4;
      free((yyvsp[-1].string));
    }
#line 2269 "yacc_sql.cpp"
    break;

  case 47: /* number: NUMBER  */
#line 533 "yacc_sql.y"
           {(yyval.number) = (yyvsp[0].number);}
#line 2275 "yacc_sql.cpp"
    break;

  case 48: /* type: INT_T  */
#line 536 "yacc_sql.y"
               { (yyval.number)=INTS; }
#line 2281 "yacc_sql.cpp"
    break;

  case 49: /* type: STRING_T  */
#line 537 "yacc_sql.y"
               { (yyval.number)=CHARS; }
#line 2287 "yacc_sql.cpp"
    break;

  case 50: /* type: FLOAT_T  */
#line 538 "yacc_sql.y"
               { (yyval.number)=FLOATS; }
#line 2293 "yacc_sql.cpp"
    break;

  case 51: /* type: DATE_T  */
#line 539 "yacc_sql.y"
               { (yyval.number)=DATES; }
#line 2299 "yacc_sql.cpp"
    break;

  case 52: /* insert_stmt: INSERT INTO ID VALUES value_row value_rows  */
#line 543 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_INSERT);
      (yyval.sql_node)->insertion.relation_name = (yyvsp[-3].string);
      if ((yyvsp[0].value_rows) != nullptr) {
        (yyval.sql_node)->insertion.values.swap(*(yyvsp[0].value_rows));
      }
      std::copy((yyvsp[-1].value_row)->begin(), (yyvsp[-1].value_row)->end(), std::back_inserter((yyval.sql_node)->insertion.values));
      std::reverse((yyval.sql_node)->insertion.values.begin(), (yyval.sql_node)->insertion.values.end());
      delete (yyvsp[-1].value_row);
      free((yyvsp[-3].string));
    }
#line 2315 "yacc_sql.cpp"
    break;

  case 53: /* value_rows: %empty  */
#line 558 "yacc_sql.y"
    {
      (yyval.value_rows) = nullptr;
    }
#line 2323 "yacc_sql.cpp"
    break;

  case 54: /* value_rows: COMMA value_row value_rows  */
#line 561 "yacc_sql.y"
                                { 
      if ((yyvsp[0].value_rows) != nullptr) {
        (yyval.value_rows) = (yyvsp[0].value_rows);
      } else {
        (yyval.value_rows) = new std::vector<Value>;
      }
      std::copy((yyvsp[-1].value_row)->begin(), (yyvsp[-1].value_row)->end(), std::back_inserter(*(yyval.value_rows)));
      delete (yyvsp[-1].value_row);
    }
#line 2337 "yacc_sql.cpp"
    break;

  case 55: /* value_row: LBRACE value value_list RBRACE  */
#line 574 "yacc_sql.y"
    {
      (yyval.value_row) = new std::vector<Value>;
      if ((yyvsp[-1].value_list) != nullptr) {
        (yyval.value_row)->swap(*(yyvsp[-1].value_list));
      }
      (yyval.value_row)->emplace_back(*(yyvsp[-2].value));
      delete (yyvsp[-2].value);
    }
#line 2350 "yacc_sql.cpp"
    break;

  case 56: /* value_list: %empty  */
#line 586 "yacc_sql.y"
    {
      (yyval.value_list) = nullptr;
    }
#line 2358 "yacc_sql.cpp"
    break;

  case 57: /* value_list: COMMA value value_list  */
#line 589 "yacc_sql.y"
                              { 
      if ((yyvsp[0].value_list) != nullptr) {
        (yyval.value_list) = (yyvsp[0].value_list);
      } else {
        (yyval.value_list) = new std::vector<Value>;
      }
      (yyval.value_list)->emplace_back(*(yyvsp[-1].value));
      delete (yyvsp[-1].value);
    }
#line 2372 "yacc_sql.cpp"
    break;

  case 58: /* value: NUMBER  */
#line 600 "yacc_sql.y"
           {
      (yyval.value) = new Value((int)(yyvsp[0].number));
      (yyloc) = (yylsp[0]);
    }
#line 2381 "yacc_sql.cpp"
    break;

  case 59: /* value: FLOAT  */
#line 604 "yacc_sql.y"
           {
      (yyval.value) = new Value((float)(yyvsp[0].floats));
      (yyloc) = (yylsp[0]);
    }
#line 2390 "yacc_sql.cpp"
    break;

  case 60: /* value: DATE  */
#line 608 "yacc_sql.y"
          {
      char *tmp = common::substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
      (yyval.value) = new Value((Date)tmp);
      free(tmp);
      if (!(yyval.value)->get_date().check()){
        YYABORT;
      }
    }
#line 2403 "yacc_sql.cpp"
    break;

  case 61: /* value: SSS  */
#line 616 "yacc_sql.y"
         {
      
        char *tmp = common::substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
        (yyval.value) = new Value(tmp);
        free(tmp);
      
    }
#line 2415 "yacc_sql.cpp"
    break;

  case 62: /* value: NULLVALUE  */
#line 623 "yacc_sql.y"
              {
      (yyval.value) = new Value();
      (yyval.value)->set_type(AttrType::NULLTYPE);
      (yyval.value)->set_nullable(true);
      (yyval.value)->set_is_null(true);
    }
#line 2426 "yacc_sql.cpp"
    break;

  case 63: /* delete_stmt: DELETE FROM ID where  */
#line 634 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DELETE);
      (yyval.sql_node)->deletion.relation_name = (yyvsp[-1].string);
      if ((yyvsp[0].condition_list) != nullptr) {
        (yyval.sql_node)->deletion.conditions.swap(*(yyvsp[0].condition_list));
        delete (yyvsp[0].condition_list);
      }
      free((yyvsp[-1].string));
    }
#line 2440 "yacc_sql.cpp"
    break;

  case 64: /* update_stmt: UPDATE ID SET ID EQ value set_list where  */
#line 646 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_UPDATE);
      if ((yyvsp[-1].set_list) != nullptr) {
        (yyval.sql_node)->update.attr_value_pairs.swap(*(yyvsp[-1].set_list));
      }
      (yyval.sql_node)->update.relation_name = (yyvsp[-6].string);
      (yyval.sql_node)->update.attr_value_pairs.emplace_back(AttrValuePair{(yyvsp[-4].string), *(yyvsp[-2].value), nullptr});
      if ((yyvsp[0].condition_list) != nullptr) {
        (yyval.sql_node)->update.conditions.swap(*(yyvsp[0].condition_list));
        delete (yyvsp[0].condition_list);
      }
      free((yyvsp[-6].string));
      free((yyvsp[-4].string));
    }
#line 2459 "yacc_sql.cpp"
    break;

  case 65: /* update_stmt: UPDATE ID SET ID EQ LBRACE select_stmt RBRACE set_list where  */
#line 661 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_UPDATE);
      if ((yyvsp[-1].set_list) != nullptr) {
        (yyval.sql_node)->update.attr_value_pairs.swap(*(yyvsp[-1].set_list));
      }
      (yyval.sql_node)->update.relation_name = (yyvsp[-8].string);
      (yyval.sql_node)->update.attr_value_pairs.emplace_back(AttrValuePair{(yyvsp[-6].string), Value(), &(yyvsp[-3].sql_node)->selection});
     
      if ((yyvsp[0].condition_list) != nullptr) {
        (yyval.sql_node)->update.conditions.swap(*(yyvsp[0].condition_list));
        delete (yyvsp[0].condition_list);
      }
      free((yyvsp[-8].string));
      free((yyvsp[-6].string));
    }
#line 2479 "yacc_sql.cpp"
    break;

  case 66: /* set_list: %empty  */
#line 679 "yacc_sql.y"
    {
      (yyval.set_list) = nullptr;
    }
#line 2487 "yacc_sql.cpp"
    break;

  case 67: /* set_list: COMMA ID EQ value set_list  */
#line 683 "yacc_sql.y"
    {
      if ((yyvsp[0].set_list) != nullptr) {
        (yyval.set_list) = (yyvsp[0].set_list);
      } else {
        (yyval.set_list) = new std::vector<AttrValuePair>;
      }
      (yyval.set_list)->emplace_back(AttrValuePair{(yyvsp[-3].string), *(yyvsp[-1].value), nullptr});
      delete (yyvsp[-3].string);
      delete (yyvsp[-1].value);
    }
#line 2502 "yacc_sql.cpp"
    break;

  case 68: /* set_list: COMMA ID EQ LBRACE select_stmt RBRACE set_list  */
#line 694 "yacc_sql.y"
    {
      if ((yyvsp[0].set_list) != nullptr) {
        (yyval.set_list) = (yyvsp[0].set_list);
      } else {
        (yyval.set_list) = new std::vector<AttrValuePair>;
      }
      (yyval.set_list)->emplace_back(AttrValuePair{(yyvsp[-5].string), Value(), &(yyvsp[-2].sql_node)->selection});
      delete (yyvsp[-5].string);
    }
#line 2516 "yacc_sql.cpp"
    break;

  case 69: /* select_stmt: SELECT select_attr FROM tables where group_by_columns having order_list  */
#line 705 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SELECT);
      if ((yyvsp[-6].rel_attr_list) != nullptr) {
        for (RelAttrSqlNode rel : *(yyvsp[-6].rel_attr_list)) {
          (yyval.sql_node)->selection.column_alias.emplace_back(rel.column_alias);
        }
        (yyval.sql_node)->selection.attributes.swap(*(yyvsp[-6].rel_attr_list));
        delete (yyvsp[-6].rel_attr_list);
      }
      for(std::string &table : *(yyvsp[-4].relation_list)) {
        int pos;
        if ((pos = table.find(" ")) != table.npos) {
          std::string tmp = table;
          table = tmp.substr(0, pos);
          std::string alias = tmp.substr(pos + 1);
          (yyval.sql_node)->selection.table_alias.insert(std::pair<std::string, std::string>(alias, table));
          table = alias;
        }
      }
      (yyval.sql_node)->selection.relations.swap(*(yyvsp[-4].relation_list));
      delete (yyvsp[-4].relation_list);
      std::reverse((yyval.sql_node)->selection.relations.begin(), (yyval.sql_node)->selection.relations.end());

      if ((yyvsp[-3].condition_list) != nullptr) {
        (yyval.sql_node)->selection.conditions.swap(*(yyvsp[-3].condition_list));
        delete (yyvsp[-3].condition_list);
      }

      if((yyvsp[-2].relation_list) != nullptr) {
        (yyval.sql_node)->selection.group_by_columns.swap(*(yyvsp[-2].relation_list));
        std::reverse((yyval.sql_node)->selection.group_by_columns.begin(), (yyval.sql_node)->selection.group_by_columns.end());
        delete (yyvsp[-2].relation_list);
      }

      if((yyvsp[-1].condition_list) != nullptr) {
        (yyval.sql_node)->selection.having_conditions.swap(*(yyvsp[-1].condition_list));
        delete (yyvsp[-1].condition_list);
      }

      if((yyvsp[0].unordered_list) != nullptr) {
        (yyval.sql_node)->selection.order_columns.swap(*(yyvsp[0].unordered_list));
        delete (yyvsp[0].unordered_list);
      }
    }
#line 2565 "yacc_sql.cpp"
    break;

  case 70: /* select_stmt: SELECT select_attr FROM complete_inner_join_node where group_by_columns having order_list  */
#line 750 "yacc_sql.y"
    {
      if ((yyvsp[-4].sql_node) != nullptr){
        (yyval.sql_node) = (yyvsp[-4].sql_node);
      } else {
        (yyval.sql_node) = new ParsedSqlNode(SCF_SELECT);
      }
      std::reverse((yyval.sql_node)->selection.relations.begin(), (yyval.sql_node)->selection.relations.end());

      if ((yyvsp[-6].rel_attr_list) != nullptr) {
        for (RelAttrSqlNode rel : *(yyvsp[-6].rel_attr_list)) {
          (yyval.sql_node)->selection.column_alias.emplace_back(rel.column_alias);
        }
        (yyval.sql_node)->selection.attributes.swap(*(yyvsp[-6].rel_attr_list));
        delete (yyvsp[-6].rel_attr_list);
      }

      if ((yyvsp[-3].condition_list) != nullptr) {
        std::copy((yyvsp[-3].condition_list)->begin(), (yyvsp[-3].condition_list)->end(), std::back_inserter((yyval.sql_node)->selection.conditions));
        delete (yyvsp[-3].condition_list);
      }

      if((yyvsp[-2].relation_list) != nullptr) {
        (yyval.sql_node)->selection.group_by_columns.swap(*(yyvsp[-2].relation_list));
        std::reverse((yyval.sql_node)->selection.group_by_columns.begin(), (yyval.sql_node)->selection.group_by_columns.end());
        delete (yyvsp[-2].relation_list);
      }

      if((yyvsp[-1].condition_list) != nullptr) {
        (yyval.sql_node)->selection.having_conditions.swap(*(yyvsp[-1].condition_list));
        delete (yyvsp[-1].condition_list);
      }

      if((yyvsp[0].unordered_list) != nullptr) {
        (yyval.sql_node)->selection.order_columns.swap(*(yyvsp[0].unordered_list));
        delete (yyvsp[0].unordered_list);
      }
    }
#line 2607 "yacc_sql.cpp"
    break;

  case 71: /* calc_stmt: CALC expression_list  */
#line 790 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CALC);
      std::reverse((yyvsp[0].expression_list)->begin(), (yyvsp[0].expression_list)->end());
      (yyval.sql_node)->calc.expressions.swap(*(yyvsp[0].expression_list));
      delete (yyvsp[0].expression_list);
    }
#line 2618 "yacc_sql.cpp"
    break;

  case 72: /* calc_stmt: SELECT expression_list  */
#line 797 "yacc_sql.y"
    {
      //std::cout << "calc" << std::endl;
      (yyval.sql_node) = new ParsedSqlNode(SCF_CALC);
      std::reverse((yyvsp[0].expression_list)->begin(), (yyvsp[0].expression_list)->end());
      (yyval.sql_node)->calc.expressions.swap(*(yyvsp[0].expression_list));
      delete (yyvsp[0].expression_list);
    }
#line 2630 "yacc_sql.cpp"
    break;

  case 73: /* part_expression_list: %empty  */
#line 808 "yacc_sql.y"
    {
      (yyval.expression_list) = nullptr;
    }
#line 2638 "yacc_sql.cpp"
    break;

  case 74: /* part_expression_list: COMMA expression part_expression_list  */
#line 812 "yacc_sql.y"
    {
      if ((yyvsp[0].expression_list) != nullptr) {
        (yyval.expression_list) = (yyvsp[0].expression_list);
      } else {
        (yyval.expression_list) = new std::vector<Expression *>;
      }

      (yyval.expression_list)->emplace_back((yyvsp[-1].expression));
    }
#line 2652 "yacc_sql.cpp"
    break;

  case 75: /* part_expression_list: COMMA expression ID part_expression_list  */
#line 822 "yacc_sql.y"
    {
      if ((yyvsp[0].expression_list) != nullptr) {
        (yyval.expression_list) = (yyvsp[0].expression_list);
      } else {
        (yyval.expression_list) = new std::vector<Expression *>;
      }
      (yyvsp[-2].expression)->set_name((yyvsp[-1].string));
      (yyval.expression_list)->emplace_back((yyvsp[-2].expression));
    }
#line 2666 "yacc_sql.cpp"
    break;

  case 76: /* part_expression_list: COMMA expression AS ID part_expression_list  */
#line 832 "yacc_sql.y"
    {
      if ((yyvsp[0].expression_list) != nullptr) {
        (yyval.expression_list) = (yyvsp[0].expression_list);
      } else {
        (yyval.expression_list) = new std::vector<Expression *>;
      }
      (yyvsp[-3].expression)->set_name((yyvsp[-1].string));
      (yyval.expression_list)->emplace_back((yyvsp[-3].expression));
    }
#line 2680 "yacc_sql.cpp"
    break;

  case 77: /* expression_list: expression part_expression_list  */
#line 845 "yacc_sql.y"
    {
      if ((yyvsp[0].expression_list) != nullptr) {
        (yyval.expression_list) = (yyvsp[0].expression_list);
      } else {
        (yyval.expression_list) = new std::vector<Expression*>;
      }

      (yyval.expression_list)->emplace_back((yyvsp[-1].expression));
    }
#line 2694 "yacc_sql.cpp"
    break;

  case 78: /* expression_list: expression ID part_expression_list  */
#line 855 "yacc_sql.y"
    {
      if ((yyvsp[0].expression_list) != nullptr) {
        (yyval.expression_list) = (yyvsp[0].expression_list);
      } else {
        (yyval.expression_list) = new std::vector<Expression*>;
      }

      (yyvsp[-2].expression)->set_name((yyvsp[-1].string));
      (yyval.expression_list)->emplace_back((yyvsp[-2].expression));
    }
#line 2709 "yacc_sql.cpp"
    break;

  case 79: /* expression_list: expression AS ID part_expression_list  */
#line 866 "yacc_sql.y"
    {
      if ((yyvsp[0].expression_list) != nullptr) {
        (yyval.expression_list) = (yyvsp[0].expression_list);
      } else {
        (yyval.expression_list) = new std::vector<Expression*>;
      }

      (yyvsp[-3].expression)->set_name((yyvsp[-1].string));
      (yyval.expression_list)->emplace_back((yyvsp[-3].expression));
    }
#line 2724 "yacc_sql.cpp"
    break;

  case 80: /* expression: expression '+' expression  */
#line 878 "yacc_sql.y"
                              {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::ADD, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2732 "yacc_sql.cpp"
    break;

  case 81: /* expression: expression '-' expression  */
#line 881 "yacc_sql.y"
                                {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::SUB, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2740 "yacc_sql.cpp"
    break;

  case 82: /* expression: expression '*' expression  */
#line 884 "yacc_sql.y"
                                {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::MUL, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2748 "yacc_sql.cpp"
    break;

  case 83: /* expression: expression '/' expression  */
#line 887 "yacc_sql.y"
                                {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::DIV, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2756 "yacc_sql.cpp"
    break;

  case 84: /* expression: LBRACE expression RBRACE  */
#line 890 "yacc_sql.y"
                               {
      (yyval.expression) = (yyvsp[-1].expression);
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
    }
#line 2765 "yacc_sql.cpp"
    break;

  case 85: /* expression: LENGTH LBRACE expression RBRACE  */
#line 894 "yacc_sql.y"
                                      {
      (yyval.expression) = create_function_expression(FunctionExpr::Type::LENGTH, (yyvsp[-1].expression), sql_string, &(yyloc));
      //std::cout << "length" << std::endl;
    }
#line 2774 "yacc_sql.cpp"
    break;

  case 86: /* expression: ROUND LBRACE expression RBRACE  */
#line 898 "yacc_sql.y"
                                     {
      (yyval.expression) = create_function_expression(FunctionExpr::Type::ROUND, (yyvsp[-1].expression), 0, sql_string, &(yyloc));
    }
#line 2782 "yacc_sql.cpp"
    break;

  case 87: /* expression: ROUND LBRACE expression COMMA NUMBER RBRACE  */
#line 901 "yacc_sql.y"
                                                  {
      (yyval.expression) = create_function_expression(FunctionExpr::Type::ROUND, (yyvsp[-3].expression), (yyvsp[-1].number), sql_string, &(yyloc));
    }
#line 2790 "yacc_sql.cpp"
    break;

  case 88: /* expression: DATE_FORMAT LBRACE expression COMMA SSS RBRACE  */
#line 904 "yacc_sql.y"
                                                     {
      char *tmp = common::substr((yyvsp[-1].string),1,strlen((yyvsp[-1].string))-2);
      (yyval.expression) = create_function_expression(FunctionExpr::Type::DATE_FORMAT, (yyvsp[-3].expression), tmp, sql_string, &(yyloc));
      free(tmp);
    }
#line 2800 "yacc_sql.cpp"
    break;

  case 89: /* expression: '-' expression  */
#line 909 "yacc_sql.y"
                                  {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::NEGATIVE, (yyvsp[0].expression), nullptr, sql_string, &(yyloc));
    }
#line 2808 "yacc_sql.cpp"
    break;

  case 90: /* expression: expression NUMBER  */
#line 912 "yacc_sql.y"
                        {
      Expression *num_expr = new ValueExpr(Value((yyvsp[0].number)));
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::ADD, (yyvsp[-1].expression), num_expr, sql_string, &(yyloc));
    }
#line 2817 "yacc_sql.cpp"
    break;

  case 91: /* expression: value  */
#line 916 "yacc_sql.y"
            {
      //std::cout << "value" << std::endl;
      (yyval.expression) = new ValueExpr(*(yyvsp[0].value));
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
      delete (yyvsp[0].value);
    }
#line 2828 "yacc_sql.cpp"
    break;

  case 92: /* expression: rel_attr  */
#line 922 "yacc_sql.y"
               {
      (yyval.expression) = new FieldExpr(*(yyvsp[0].rel_attr));
      (yyval.expression)->set_name("");
      //$$->set_name(token_name(sql_string, &@$));
      delete (yyvsp[0].rel_attr);
    }
#line 2839 "yacc_sql.cpp"
    break;

  case 93: /* select_attr: expression_list  */
#line 932 "yacc_sql.y"
    {
      (yyval.rel_attr_list) = new std::vector<RelAttrSqlNode>;
      for (auto &expr : *(yyvsp[0].expression_list)) {
        RelAttrSqlNode rel_attr_node;
        //std::cout << "field ex" << std::endl;
        field_extract(expr, rel_attr_node);
        rel_attr_node.expr = expr;
        (yyval.rel_attr_list)->emplace_back(rel_attr_node);
      }
     
      delete (yyvsp[0].expression_list);
    }
#line 2856 "yacc_sql.cpp"
    break;

  case 94: /* select_attr: '*'  */
#line 980 "yacc_sql.y"
          {
      (yyval.rel_attr_list) = new std::vector<RelAttrSqlNode>;
      RelAttrSqlNode attr;
      attr.relation_name  = "";
      attr.attribute_name = "*";
      attr.expr = nullptr;
      (yyval.rel_attr_list)->emplace_back(attr);
      
    }
#line 2870 "yacc_sql.cpp"
    break;

  case 95: /* rel_attr: ID  */
#line 992 "yacc_sql.y"
       {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->attribute_name = (yyvsp[0].string);
      free((yyvsp[0].string));
    }
#line 2880 "yacc_sql.cpp"
    break;

  case 96: /* rel_attr: ID DOT ID  */
#line 997 "yacc_sql.y"
                {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->relation_name  = (yyvsp[-2].string);
      (yyval.rel_attr)->attribute_name = (yyvsp[0].string);
      free((yyvsp[-2].string));
      free((yyvsp[0].string));
    }
#line 2892 "yacc_sql.cpp"
    break;

  case 97: /* rel_attr: ID DOT '*'  */
#line 1004 "yacc_sql.y"
                 {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->relation_name  = (yyvsp[-2].string);
      (yyval.rel_attr)->attribute_name = '*';
      free((yyvsp[-2].string));
    }
#line 2903 "yacc_sql.cpp"
    break;

  case 98: /* rel_attr: func LBRACE ID RBRACE  */
#line 1010 "yacc_sql.y"
                            {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->attribute_name = (yyvsp[-1].string);
      (yyval.rel_attr)->func = (yyvsp[-3].func);
      free((yyvsp[-1].string));
    }
#line 2914 "yacc_sql.cpp"
    break;

  case 99: /* rel_attr: func LBRACE '*' RBRACE  */
#line 1016 "yacc_sql.y"
                             {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->attribute_name = "*";
      if ((yyvsp[-3].func) != AggregationFunc::COUNTFUN)
        YYABORT;
      (yyval.rel_attr)->func = (yyvsp[-3].func);
    }
#line 2926 "yacc_sql.cpp"
    break;

  case 100: /* rel_attr: func LBRACE ID DOT ID RBRACE  */
#line 1023 "yacc_sql.y"
                                   {
      (yyval.rel_attr) = new RelAttrSqlNode;;
      (yyval.rel_attr)->relation_name = (yyvsp[-3].string);
      (yyval.rel_attr)->attribute_name = (yyvsp[-1].string);
      (yyval.rel_attr)->func = (yyvsp[-5].func);
      free((yyvsp[-3].string));
      free((yyvsp[-1].string));
    }
#line 2939 "yacc_sql.cpp"
    break;

  case 101: /* order_attr: rel_attr  */
#line 1034 "yacc_sql.y"
             {
      (yyval.pair) = new std::pair<RelAttrSqlNode, bool>;
      (yyval.pair)->first = *(yyvsp[0].rel_attr);
      (yyval.pair)->second = true;
      delete (yyvsp[0].rel_attr);
    }
#line 2950 "yacc_sql.cpp"
    break;

  case 102: /* order_attr: rel_attr ASC  */
#line 1040 "yacc_sql.y"
                   {
      (yyval.pair) = new std::pair<RelAttrSqlNode, bool>;
      (yyval.pair)->first = *(yyvsp[-1].rel_attr);
      (yyval.pair)->second = true;
      delete (yyvsp[-1].rel_attr);
    }
#line 2961 "yacc_sql.cpp"
    break;

  case 103: /* order_attr: rel_attr DESC  */
#line 1046 "yacc_sql.y"
                    {
      (yyval.pair) = new std::pair<RelAttrSqlNode, bool>;
      (yyval.pair)->first = *(yyvsp[-1].rel_attr);
      (yyval.pair)->second = false;
      delete (yyvsp[-1].rel_attr);
    }
#line 2972 "yacc_sql.cpp"
    break;

  case 104: /* agg_attr: func LBRACE ID RBRACE  */
#line 1055 "yacc_sql.y"
                          {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->attribute_name = (yyvsp[-1].string);
      (yyval.rel_attr)->func = (yyvsp[-3].func);
      free((yyvsp[-1].string));
    }
#line 2983 "yacc_sql.cpp"
    break;

  case 105: /* agg_attr: func LBRACE '*' RBRACE  */
#line 1061 "yacc_sql.y"
                             {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->attribute_name = "*";
      if ((yyvsp[-3].func) != AggregationFunc::COUNTFUN)
        YYABORT;
      (yyval.rel_attr)->func = (yyvsp[-3].func);
    }
#line 2995 "yacc_sql.cpp"
    break;

  case 106: /* agg_attr: func LBRACE ID DOT ID RBRACE  */
#line 1068 "yacc_sql.y"
                                   {
      (yyval.rel_attr) = new RelAttrSqlNode;;
      (yyval.rel_attr)->relation_name = (yyvsp[-3].string);
      (yyval.rel_attr)->attribute_name = (yyvsp[-1].string);
      (yyval.rel_attr)->func = (yyvsp[-5].func);
      free((yyvsp[-3].string));
      free((yyvsp[-1].string));
    }
#line 3008 "yacc_sql.cpp"
    break;

  case 107: /* unordered_list: %empty  */
#line 1124 "yacc_sql.y"
    {
      (yyval.unordered_list) = nullptr;
    }
#line 3016 "yacc_sql.cpp"
    break;

  case 108: /* unordered_list: COMMA order_attr unordered_list  */
#line 1128 "yacc_sql.y"
    {
      if ((yyvsp[0].unordered_list) != nullptr) {
        (yyval.unordered_list) = (yyvsp[0].unordered_list);
      } else {
        (yyval.unordered_list) = new std::vector<std::pair<RelAttrSqlNode, bool>>;
      }
      
      (yyval.unordered_list)->emplace_back(*(yyvsp[-1].pair));
      delete (yyvsp[-1].pair);
    }
#line 3031 "yacc_sql.cpp"
    break;

  case 109: /* order_list: %empty  */
#line 1142 "yacc_sql.y"
    {
      (yyval.unordered_list) = nullptr;
    }
#line 3039 "yacc_sql.cpp"
    break;

  case 110: /* order_list: ORDERBY order_attr unordered_list  */
#line 1146 "yacc_sql.y"
    {
      if ((yyvsp[0].unordered_list) == nullptr) {
        (yyval.unordered_list) = new std::vector<std::pair<RelAttrSqlNode, bool>>;
      } else {
        (yyval.unordered_list) = (yyvsp[0].unordered_list);
      }

      (yyval.unordered_list)->emplace_back(*(yyvsp[-1].pair));
      delete (yyvsp[-1].pair);
    }
#line 3054 "yacc_sql.cpp"
    break;

  case 111: /* rel_list: %empty  */
#line 1160 "yacc_sql.y"
    {
      (yyval.relation_list) = nullptr;
    }
#line 3062 "yacc_sql.cpp"
    break;

  case 112: /* rel_list: COMMA ID rel_list  */
#line 1163 "yacc_sql.y"
                        {
      if ((yyvsp[0].relation_list) != nullptr) {
        (yyval.relation_list) = (yyvsp[0].relation_list);
      } else {
        (yyval.relation_list) = new std::vector<std::string>;
      }

      (yyval.relation_list)->push_back((yyvsp[-1].string));
      free((yyvsp[-1].string));
    }
#line 3077 "yacc_sql.cpp"
    break;

  case 113: /* rel_list: COMMA ID ID rel_list  */
#line 1173 "yacc_sql.y"
                           {
      if ((yyvsp[0].relation_list) != nullptr) {
        (yyval.relation_list) = (yyvsp[0].relation_list);
      } else {
        (yyval.relation_list) = new std::vector<std::string>;
      }

      std::string name((yyvsp[-2].string));
      name = name + " " + (yyvsp[-1].string);
      (yyval.relation_list)->push_back(name);
      free((yyvsp[-2].string));
      free((yyvsp[-1].string));
    }
#line 3095 "yacc_sql.cpp"
    break;

  case 114: /* rel_list: COMMA ID AS ID rel_list  */
#line 1186 "yacc_sql.y"
                              {
      if ((yyvsp[0].relation_list) != nullptr) {
        (yyval.relation_list) = (yyvsp[0].relation_list);
      } else {
        (yyval.relation_list) = new std::vector<std::string>;
      }

      std::string name((yyvsp[-3].string));
      name = name + " " + (yyvsp[-1].string);
      (yyval.relation_list)->push_back(name);
      free((yyvsp[-3].string));
      free((yyvsp[-1].string));
    }
#line 3113 "yacc_sql.cpp"
    break;

  case 115: /* tables: ID rel_list  */
#line 1202 "yacc_sql.y"
                {
      if ((yyvsp[0].relation_list) != nullptr) {
        (yyval.relation_list) = (yyvsp[0].relation_list);
      } else {
        (yyval.relation_list) = new std::vector<std::string>;
      }
      (yyval.relation_list)->push_back((yyvsp[-1].string));
      free((yyvsp[-1].string));
    }
#line 3127 "yacc_sql.cpp"
    break;

  case 116: /* tables: ID ID rel_list  */
#line 1211 "yacc_sql.y"
                     {
      if ((yyvsp[0].relation_list) != nullptr) {
        (yyval.relation_list) = (yyvsp[0].relation_list);
      } else {
        (yyval.relation_list) = new std::vector<std::string>;
      }
      std::string name((yyvsp[-2].string));
      name = name + " " + (yyvsp[-1].string);
      (yyval.relation_list)->push_back(name);
      free((yyvsp[-2].string));
      free((yyvsp[-1].string));
    }
#line 3144 "yacc_sql.cpp"
    break;

  case 117: /* tables: ID AS ID rel_list  */
#line 1223 "yacc_sql.y"
                        {
      if ((yyvsp[0].relation_list) != nullptr) {
        (yyval.relation_list) = (yyvsp[0].relation_list);
      } else {
        (yyval.relation_list) = new std::vector<std::string>;
      }
      std::string name((yyvsp[-3].string));
      name = name + " " + (yyvsp[-1].string);
      (yyval.relation_list)->push_back(name);
      free((yyvsp[-3].string));
      free((yyvsp[-1].string));
    }
#line 3161 "yacc_sql.cpp"
    break;

  case 118: /* func: MAX  */
#line 1238 "yacc_sql.y"
        {
      (yyval.func) = AggregationFunc::MAXFUN;
    }
#line 3169 "yacc_sql.cpp"
    break;

  case 119: /* func: MIN  */
#line 1241 "yacc_sql.y"
          {
      (yyval.func) = AggregationFunc::MINFUN;
    }
#line 3177 "yacc_sql.cpp"
    break;

  case 120: /* func: COUNT  */
#line 1244 "yacc_sql.y"
            {
      (yyval.func) = AggregationFunc::COUNTFUN;
    }
#line 3185 "yacc_sql.cpp"
    break;

  case 121: /* func: AVG  */
#line 1247 "yacc_sql.y"
          {
      (yyval.func) = AggregationFunc::AVGFUN;
    }
#line 3193 "yacc_sql.cpp"
    break;

  case 122: /* func: SUM  */
#line 1250 "yacc_sql.y"
          {
      (yyval.func) = AggregationFunc::SUMFUN;
    }
#line 3201 "yacc_sql.cpp"
    break;

  case 123: /* where: %empty  */
#line 1256 "yacc_sql.y"
    {
      (yyval.condition_list) = nullptr;
    }
#line 3209 "yacc_sql.cpp"
    break;

  case 124: /* where: WHERE condition_list  */
#line 1259 "yacc_sql.y"
                           {
      (yyval.condition_list) = (yyvsp[0].condition_list);  
    }
#line 3217 "yacc_sql.cpp"
    break;

  case 125: /* having: %empty  */
#line 1265 "yacc_sql.y"
    {
      (yyval.condition_list) = nullptr;
    }
#line 3225 "yacc_sql.cpp"
    break;

  case 126: /* having: HAVING agg_condition_list  */
#line 1268 "yacc_sql.y"
                                {
      (yyval.condition_list) = (yyvsp[0].condition_list);
    }
#line 3233 "yacc_sql.cpp"
    break;

  case 127: /* condition_list: %empty  */
#line 1274 "yacc_sql.y"
    {
      (yyval.condition_list) = nullptr;
    }
#line 3241 "yacc_sql.cpp"
    break;

  case 128: /* condition_list: condition  */
#line 1277 "yacc_sql.y"
                {
      if ((yyvsp[0].condition) != nullptr) {
        (yyval.condition_list) = new std::vector<ConditionSqlNode>;
        (yyval.condition_list)->emplace_back(*(yyvsp[0].condition));
        delete (yyvsp[0].condition);
      }
      else
        (yyval.condition_list) = nullptr;
    }
#line 3255 "yacc_sql.cpp"
    break;

  case 129: /* condition_list: condition AND condition_list  */
#line 1286 "yacc_sql.y"
                                   {
      (yyval.condition_list) = (yyvsp[0].condition_list);
      if ((yyvsp[-2].condition) != nullptr){
        if ((yyval.condition_list) == nullptr)
          (yyval.condition_list) = new std::vector<ConditionSqlNode>;
        (yyval.condition_list)->emplace_back(*(yyvsp[-2].condition));
        delete (yyvsp[-2].condition);
      }
    }
#line 3269 "yacc_sql.cpp"
    break;

  case 130: /* agg_condition_list: agg_condition  */
#line 1298 "yacc_sql.y"
                  {
      if ((yyvsp[0].condition) != nullptr) {
        (yyval.condition_list) = new std::vector<ConditionSqlNode>;
        (yyval.condition_list)->emplace_back(*(yyvsp[0].condition));
        delete (yyvsp[0].condition);
      }
      else
        (yyval.condition_list) = nullptr;
    }
#line 3283 "yacc_sql.cpp"
    break;

  case 131: /* agg_condition_list: agg_condition AND agg_condition_list  */
#line 1307 "yacc_sql.y"
                                           {
      (yyval.condition_list) = (yyvsp[0].condition_list);
      if ((yyvsp[-2].condition) != nullptr){
        if ((yyval.condition_list) == nullptr)
          (yyval.condition_list) = new std::vector<ConditionSqlNode>;
        (yyval.condition_list)->emplace_back(*(yyvsp[-2].condition));
        delete (yyvsp[-2].condition);
      }
    }
#line 3297 "yacc_sql.cpp"
    break;

  case 132: /* condition: value ISNULL  */
#line 1382 "yacc_sql.y"
    {
      (yyval.condition) = always_false();

      delete (yyvsp[-1].value);
    }
#line 3307 "yacc_sql.cpp"
    break;

  case 133: /* condition: rel_attr ISNULL  */
#line 1388 "yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 1;
      (yyval.condition)->left_attr = *(yyvsp[-1].rel_attr);
      (yyval.condition)->right_is_attr = 0;
      Value null_value = Value();
      null_value.set_nullable(true);
      null_value.set_is_null(true);
      (yyval.condition)->right_value = null_value;
      (yyval.condition)->comp = CompOp::EQUAL_TO;

      delete (yyvsp[-1].rel_attr);
    }
#line 3325 "yacc_sql.cpp"
    break;

  case 134: /* condition: value ISNOTNULL  */
#line 1402 "yacc_sql.y"
    {
      (yyval.condition) = nullptr;

      delete (yyvsp[-1].value);
    }
#line 3335 "yacc_sql.cpp"
    break;

  case 135: /* condition: rel_attr ISNOTNULL  */
#line 1408 "yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 1;
      (yyval.condition)->left_attr = *(yyvsp[-1].rel_attr);
      (yyval.condition)->right_is_attr = 0;
      Value null_value = Value();
      null_value.set_nullable(true);
      null_value.set_is_null(-1);
      (yyval.condition)->right_value = null_value;
      (yyval.condition)->comp = CompOp::EQUAL_TO;

      delete (yyvsp[-1].rel_attr);
    }
#line 3353 "yacc_sql.cpp"
    break;

  case 136: /* condition: NULLISNULL  */
#line 1422 "yacc_sql.y"
    {
      (yyval.condition) = nullptr;
    }
#line 3361 "yacc_sql.cpp"
    break;

  case 137: /* condition: NULLISNOTNULL  */
#line 1426 "yacc_sql.y"
    {
      (yyval.condition) = always_false();
    }
#line 3369 "yacc_sql.cpp"
    break;

  case 138: /* condition: rel_attr IN LBRACE select_stmt RBRACE  */
#line 1431 "yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 1;
      (yyval.condition)->left_attr = *(yyvsp[-4].rel_attr);
      (yyval.condition)->right_is_attr = 2;
      (yyval.condition)->right_select = &((yyvsp[-1].sql_node)->selection);
      (yyval.condition)->comp = CompOp::IN_OP;

      delete (yyvsp[-4].rel_attr);
    }
#line 3384 "yacc_sql.cpp"
    break;

  case 139: /* condition: value IN LBRACE select_stmt RBRACE  */
#line 1442 "yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 0;
      (yyval.condition)->left_value = *(yyvsp[-4].value);
      (yyval.condition)->right_is_attr = 2;
      (yyval.condition)->right_select = &((yyvsp[-1].sql_node)->selection);
      (yyval.condition)->comp = CompOp::IN_OP;

      delete (yyvsp[-4].value);
    }
#line 3399 "yacc_sql.cpp"
    break;

  case 140: /* condition: rel_attr NOT IN LBRACE select_stmt RBRACE  */
#line 1453 "yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 1;
      (yyval.condition)->left_attr = *(yyvsp[-5].rel_attr);
      (yyval.condition)->right_is_attr = 2;
      (yyval.condition)->right_select = &((yyvsp[-1].sql_node)->selection);
      (yyval.condition)->comp = CompOp::NOT_IN_OP;

      delete (yyvsp[-5].rel_attr);
    }
#line 3414 "yacc_sql.cpp"
    break;

  case 141: /* condition: value NOT IN LBRACE select_stmt RBRACE  */
#line 1464 "yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 0;
      (yyval.condition)->left_value = *(yyvsp[-5].value);
      (yyval.condition)->right_is_attr = 2;
      (yyval.condition)->right_select = &((yyvsp[-1].sql_node)->selection);
      (yyval.condition)->comp = CompOp::NOT_IN_OP;

      delete (yyvsp[-5].value);
    }
#line 3429 "yacc_sql.cpp"
    break;

  case 142: /* condition: rel_attr IN LBRACE value value_list RBRACE  */
#line 1475 "yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      if ((yyvsp[-1].value_list) != nullptr) {
        (yyval.condition)->values.swap(*(yyvsp[-1].value_list));
      }
      (yyval.condition)->values.emplace_back(*(yyvsp[-2].value));
      std::reverse((yyval.condition)->values.begin(), (yyval.condition)->values.end());
      (yyval.condition)->left_is_attr = 1;
      (yyval.condition)->left_attr = *(yyvsp[-5].rel_attr);
      (yyval.condition)->right_is_attr = 3;
      (yyval.condition)->comp = CompOp::IN_OP;
      delete (yyvsp[-2].value);
      delete (yyvsp[-5].rel_attr);
    }
#line 3448 "yacc_sql.cpp"
    break;

  case 143: /* condition: value IN LBRACE value value_list RBRACE  */
#line 1490 "yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      if ((yyvsp[-1].value_list) != nullptr) {
        (yyval.condition)->values.swap(*(yyvsp[-1].value_list));
      }
      (yyval.condition)->values.emplace_back(*(yyvsp[-2].value));
      std::reverse((yyval.condition)->values.begin(), (yyval.condition)->values.end());
      (yyval.condition)->left_is_attr = 0;
      (yyval.condition)->left_value = *(yyvsp[-5].value);
      (yyval.condition)->right_is_attr = 3;
      (yyval.condition)->comp = CompOp::IN_OP;
      delete (yyvsp[-2].value);
      delete (yyvsp[-5].value);
    }
#line 3467 "yacc_sql.cpp"
    break;

  case 144: /* condition: rel_attr NOT IN LBRACE value value_list RBRACE  */
#line 1505 "yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      if ((yyvsp[-2].value) != nullptr) {
        (yyval.condition)->values.swap(*(yyvsp[-1].value_list));
      }
      (yyval.condition)->values.emplace_back(*(yyvsp[-2].value));
      std::reverse((yyval.condition)->values.begin(), (yyval.condition)->values.end());
      (yyval.condition)->left_is_attr = 1;
      (yyval.condition)->left_attr = *(yyvsp[-6].rel_attr);
      (yyval.condition)->right_is_attr = 3;
      (yyval.condition)->comp = CompOp::NOT_IN_OP;
      delete (yyvsp[-2].value);
      delete (yyvsp[-6].rel_attr);
    }
#line 3486 "yacc_sql.cpp"
    break;

  case 145: /* condition: value NOT IN LBRACE value value_list RBRACE  */
#line 1520 "yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      if ((yyvsp[-2].value) != nullptr) {
        (yyval.condition)->values.swap(*(yyvsp[-1].value_list));
      }
      (yyval.condition)->values.emplace_back(*(yyvsp[-2].value));
      std::reverse((yyval.condition)->values.begin(), (yyval.condition)->values.end());
      (yyval.condition)->left_is_attr = 0;
      (yyval.condition)->left_value = *(yyvsp[-6].value);
      (yyval.condition)->right_is_attr = 3;
      (yyval.condition)->comp = CompOp::NOT_IN_OP;
      delete (yyvsp[-2].value);
      delete (yyvsp[-6].value);
    }
#line 3505 "yacc_sql.cpp"
    break;

  case 146: /* condition: EXISTS LBRACE select_stmt RBRACE  */
#line 1536 "yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 2;
      (yyval.condition)->right_is_attr = 2;
      (yyval.condition)->right_select = &((yyvsp[-1].sql_node)->selection);
      (yyval.condition)->comp = CompOp::EXISTS_OP;

    }
#line 3518 "yacc_sql.cpp"
    break;

  case 147: /* condition: NOT EXISTS LBRACE select_stmt RBRACE  */
#line 1545 "yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 2;
      (yyval.condition)->right_is_attr = 2;
      (yyval.condition)->right_select = &((yyvsp[-1].sql_node)->selection);
      (yyval.condition)->comp = CompOp::NOT_EXISTS_OP;

    }
#line 3531 "yacc_sql.cpp"
    break;

  case 148: /* condition: expression comp_op LBRACE select_stmt RBRACE  */
#line 1555 "yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 4;
      (yyval.condition)->left_expr = (yyvsp[-4].expression);
      (yyval.condition)->right_is_attr = 2;
      (yyval.condition)->right_select = &((yyvsp[-1].sql_node)->selection);
      (yyval.condition)->comp = (yyvsp[-3].comp);

      
    }
#line 3546 "yacc_sql.cpp"
    break;

  case 149: /* condition: LBRACE select_stmt RBRACE comp_op rel_attr  */
#line 1566 "yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 1;
      (yyval.condition)->left_attr = *(yyvsp[0].rel_attr);
      (yyval.condition)->right_is_attr = 2;
      (yyval.condition)->right_select = &((yyvsp[-3].sql_node)->selection);
      (yyval.condition)->comp = (yyvsp[-1].comp);
      if ((yyval.condition)->comp == CompOp::GREAT_EQUAL || (yyval.condition)->comp == CompOp::GREAT_THAN) {
        (yyval.condition)->comp = (CompOp)((int)(yyval.condition)->comp - 2);
      } else if ((yyval.condition)->comp == CompOp::LESS_EQUAL || (yyval.condition)->comp == CompOp::LESS_THAN) {
        (yyval.condition)->comp = (CompOp)((int)(yyval.condition)->comp + 2);
      }
      delete (yyvsp[0].rel_attr);
    }
#line 3565 "yacc_sql.cpp"
    break;

  case 150: /* condition: expression comp_op expression  */
#line 1582 "yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 4;
      (yyval.condition)->left_expr = (yyvsp[-2].expression);
      (yyval.condition)->comp = (yyvsp[-1].comp);
      (yyval.condition)->right_is_attr = 4;
      (yyval.condition)->right_expr = (yyvsp[0].expression);
    
    }
#line 3579 "yacc_sql.cpp"
    break;

  case 151: /* agg_condition: agg_attr comp_op value  */
#line 1606 "yacc_sql.y"
    {
      if ((yyvsp[0].value)->nullable() && (yyvsp[0].value)->is_null())
        (yyval.condition) = always_false();
      else {
        (yyval.condition) = new ConditionSqlNode;
        (yyval.condition)->left_is_attr = 1;
        (yyval.condition)->left_attr = *(yyvsp[-2].rel_attr);
        (yyval.condition)->right_is_attr = 0;
        (yyval.condition)->right_value = *(yyvsp[0].value);
        (yyval.condition)->comp = (yyvsp[-1].comp);
      }

      delete (yyvsp[-2].rel_attr);
      delete (yyvsp[0].value);
    }
#line 3599 "yacc_sql.cpp"
    break;

  case 152: /* agg_condition: value comp_op agg_attr  */
#line 1622 "yacc_sql.y"
    {
      if ((yyvsp[-2].value)->nullable() && (yyvsp[-2].value)->is_null())
        (yyval.condition) = always_false();
      else {
        (yyval.condition) = new ConditionSqlNode;
        (yyval.condition)->left_is_attr = 0;
        (yyval.condition)->left_value = *(yyvsp[-2].value);
        (yyval.condition)->right_is_attr = 1;
        (yyval.condition)->right_attr = *(yyvsp[0].rel_attr);
        (yyval.condition)->comp = (yyvsp[-1].comp);
      }

      delete (yyvsp[-2].value);
      delete (yyvsp[0].rel_attr);
    }
#line 3619 "yacc_sql.cpp"
    break;

  case 153: /* comp_op: EQ  */
#line 1639 "yacc_sql.y"
         { (yyval.comp) = EQUAL_TO; }
#line 3625 "yacc_sql.cpp"
    break;

  case 154: /* comp_op: LT  */
#line 1640 "yacc_sql.y"
         { (yyval.comp) = LESS_THAN; }
#line 3631 "yacc_sql.cpp"
    break;

  case 155: /* comp_op: GT  */
#line 1641 "yacc_sql.y"
         { (yyval.comp) = GREAT_THAN; }
#line 3637 "yacc_sql.cpp"
    break;

  case 156: /* comp_op: LE  */
#line 1642 "yacc_sql.y"
         { (yyval.comp) = LESS_EQUAL; }
#line 3643 "yacc_sql.cpp"
    break;

  case 157: /* comp_op: GE  */
#line 1643 "yacc_sql.y"
         { (yyval.comp) = GREAT_EQUAL; }
#line 3649 "yacc_sql.cpp"
    break;

  case 158: /* comp_op: NE  */
#line 1644 "yacc_sql.y"
         { (yyval.comp) = NOT_EQUAL; }
#line 3655 "yacc_sql.cpp"
    break;

  case 159: /* comp_op: LIKE  */
#line 1645 "yacc_sql.y"
           { (yyval.comp) = LIKE_A; }
#line 3661 "yacc_sql.cpp"
    break;

  case 160: /* comp_op: NOT_LIKE  */
#line 1646 "yacc_sql.y"
               { (yyval.comp) = LIKE_NOT; }
#line 3667 "yacc_sql.cpp"
    break;

  case 161: /* group_by_columns: %empty  */
#line 1651 "yacc_sql.y"
    {
      (yyval.relation_list) = nullptr;
    }
#line 3675 "yacc_sql.cpp"
    break;

  case 162: /* group_by_columns: GROUPBY group_by_attr group_by_attr_list  */
#line 1655 "yacc_sql.y"
    {
      (yyval.relation_list) = new std::vector<std::string>;
      if ((yyvsp[0].relation_list) != nullptr){
        (yyval.relation_list)->swap(*(yyvsp[0].relation_list));
        delete (yyvsp[0].relation_list);
      }
      (yyval.relation_list)->push_back((yyvsp[-1].string));
      free((yyvsp[-1].string));
    }
#line 3689 "yacc_sql.cpp"
    break;

  case 163: /* group_by_attr_list: %empty  */
#line 1667 "yacc_sql.y"
    {
      (yyval.relation_list) = nullptr;
    }
#line 3697 "yacc_sql.cpp"
    break;

  case 164: /* group_by_attr_list: COMMA group_by_attr group_by_attr_list  */
#line 1670 "yacc_sql.y"
                                            {
      if ((yyvsp[0].relation_list) != nullptr){
        (yyval.relation_list) = (yyvsp[0].relation_list);
      } else {
        (yyval.relation_list) = new std::vector<std::string>;
      }

      (yyval.relation_list)->push_back((yyvsp[-1].string));
      free((yyvsp[-1].string));
    }
#line 3712 "yacc_sql.cpp"
    break;

  case 165: /* group_by_attr: ID  */
#line 1683 "yacc_sql.y"
       {
      (yyval.string) = (yyvsp[0].string);
    }
#line 3720 "yacc_sql.cpp"
    break;

  case 166: /* group_by_attr: ID DOT ID  */
#line 1686 "yacc_sql.y"
                {
      int len1 = strlen((yyvsp[-2].string));
      int len2 = strlen((yyvsp[0].string));
      int len = len1 + len2 + 2;
      char *temp = new char[len];
      temp[len1] = '.';
      temp[len - 1] = '\0';
      memcpy(temp, (yyvsp[-2].string), len1);
      memcpy(temp + len1 + 1, (yyvsp[0].string), len2);
      (yyval.string) = temp;
    }
#line 3736 "yacc_sql.cpp"
    break;

  case 167: /* inner_join_node: %empty  */
#line 1701 "yacc_sql.y"
    {
      (yyval.sql_node) = nullptr;
    }
#line 3744 "yacc_sql.cpp"
    break;

  case 168: /* inner_join_node: INNERJOIN ID inner_join_node  */
#line 1705 "yacc_sql.y"
    {      
      if ((yyvsp[0].sql_node) != nullptr){
        (yyval.sql_node) = (yyvsp[0].sql_node);
      } else {
        (yyval.sql_node) = new ParsedSqlNode(SCF_SELECT);
      }

      (yyval.sql_node)->selection.relations.push_back((yyvsp[-1].string));
      free((yyvsp[-1].string));
    }
#line 3759 "yacc_sql.cpp"
    break;

  case 169: /* inner_join_node: INNERJOIN ID ID inner_join_node  */
#line 1716 "yacc_sql.y"
    {      
      if ((yyvsp[0].sql_node) != nullptr){
        (yyval.sql_node) = (yyvsp[0].sql_node);
      } else {
        (yyval.sql_node) = new ParsedSqlNode(SCF_SELECT);
      }

      (yyval.sql_node)->selection.relations.push_back((yyvsp[-1].string));
      (yyval.sql_node)->selection.table_alias.insert(std::pair<std::string, std::string>((yyvsp[-1].string), (yyvsp[-2].string)));
      free((yyvsp[-2].string));
      free((yyvsp[-1].string));
    }
#line 3776 "yacc_sql.cpp"
    break;

  case 170: /* inner_join_node: INNERJOIN ID AS ID inner_join_node  */
#line 1729 "yacc_sql.y"
    {      
      if ((yyvsp[0].sql_node) != nullptr){
        (yyval.sql_node) = (yyvsp[0].sql_node);
      } else {
        (yyval.sql_node) = new ParsedSqlNode(SCF_SELECT);
      }

      (yyval.sql_node)->selection.relations.push_back((yyvsp[-1].string));
      (yyval.sql_node)->selection.table_alias.insert(std::pair<std::string, std::string>((yyvsp[-1].string), (yyvsp[-3].string)));
      free((yyvsp[-3].string));
      free((yyvsp[-1].string));
    }
#line 3793 "yacc_sql.cpp"
    break;

  case 171: /* inner_join_node: INNERJOIN ID ON condition_list inner_join_node  */
#line 1742 "yacc_sql.y"
    {
      if ((yyvsp[0].sql_node) != nullptr){
        (yyval.sql_node) = (yyvsp[0].sql_node);
      } else {
        (yyval.sql_node) = new ParsedSqlNode(SCF_SELECT);
      }

      if ((yyvsp[-1].condition_list) == nullptr)
        YYABORT;
      std::copy((yyvsp[-1].condition_list)->begin(), (yyvsp[-1].condition_list)->end(), std::back_inserter((yyval.sql_node)->selection.conditions));
      (yyval.sql_node)->selection.relations.push_back((yyvsp[-3].string));
      free((yyvsp[-3].string));
    }
#line 3811 "yacc_sql.cpp"
    break;

  case 172: /* inner_join_node: INNERJOIN ID ID ON condition_list inner_join_node  */
#line 1756 "yacc_sql.y"
    {
      if ((yyvsp[0].sql_node) != nullptr){
        (yyval.sql_node) = (yyvsp[0].sql_node);
      } else {
        (yyval.sql_node) = new ParsedSqlNode(SCF_SELECT);
      }

      if ((yyvsp[-1].condition_list) == nullptr)
        YYABORT;
      std::copy((yyvsp[-1].condition_list)->begin(), (yyvsp[-1].condition_list)->end(), std::back_inserter((yyval.sql_node)->selection.conditions));
      (yyval.sql_node)->selection.relations.push_back((yyvsp[-3].string));
      (yyval.sql_node)->selection.table_alias.insert(std::pair<std::string, std::string>((yyvsp[-3].string), (yyvsp[-4].string)));
      free((yyvsp[-4].string));
      free((yyvsp[-3].string));
    }
#line 3831 "yacc_sql.cpp"
    break;

  case 173: /* inner_join_node: INNERJOIN ID AS ID ON condition_list inner_join_node  */
#line 1772 "yacc_sql.y"
    {
      if ((yyvsp[0].sql_node) != nullptr){
        (yyval.sql_node) = (yyvsp[0].sql_node);
      } else {
        (yyval.sql_node) = new ParsedSqlNode(SCF_SELECT);
      }

      if ((yyvsp[-1].condition_list) == nullptr)
        YYABORT;
      std::copy((yyvsp[-1].condition_list)->begin(), (yyvsp[-1].condition_list)->end(), std::back_inserter((yyval.sql_node)->selection.conditions));
      (yyval.sql_node)->selection.relations.push_back((yyvsp[-3].string));
      (yyval.sql_node)->selection.table_alias.insert(std::pair<std::string, std::string>((yyvsp[-3].string), (yyvsp[-5].string)));
      free((yyvsp[-5].string));
      free((yyvsp[-3].string));
    }
#line 3851 "yacc_sql.cpp"
    break;

  case 174: /* complete_inner_join_node: ID inner_join_node  */
#line 1791 "yacc_sql.y"
    {
      if ((yyvsp[0].sql_node) != nullptr) {
        (yyval.sql_node) = (yyvsp[0].sql_node);
      } else {
        (yyval.sql_node) = new ParsedSqlNode(SCF_SELECT);
      }

      (yyval.sql_node)->selection.relations.push_back((yyvsp[-1].string));
      free((yyvsp[-1].string));
    }
#line 3866 "yacc_sql.cpp"
    break;

  case 175: /* complete_inner_join_node: ID ID inner_join_node  */
#line 1802 "yacc_sql.y"
    {
      if ((yyvsp[0].sql_node) != nullptr) {
        (yyval.sql_node) = (yyvsp[0].sql_node);
      } else {
        (yyval.sql_node) = new ParsedSqlNode(SCF_SELECT);
      }

      (yyval.sql_node)->selection.relations.push_back((yyvsp[-1].string));
      (yyval.sql_node)->selection.table_alias.insert(std::pair<std::string, std::string>((yyvsp[-1].string), (yyvsp[-2].string)));

      free((yyvsp[-2].string));
      free((yyvsp[-1].string));
    }
#line 3884 "yacc_sql.cpp"
    break;

  case 176: /* complete_inner_join_node: ID AS ID inner_join_node  */
#line 1816 "yacc_sql.y"
    {
      if ((yyvsp[0].sql_node) != nullptr) {
        (yyval.sql_node) = (yyvsp[0].sql_node);
      } else {
        (yyval.sql_node) = new ParsedSqlNode(SCF_SELECT);
      }

      (yyval.sql_node)->selection.relations.push_back((yyvsp[-1].string));
      (yyval.sql_node)->selection.table_alias.insert(std::pair<std::string, std::string>((yyvsp[-1].string), (yyvsp[-3].string)));

      free((yyvsp[-3].string));
      free((yyvsp[-1].string));
    }
#line 3902 "yacc_sql.cpp"
    break;

  case 177: /* load_data_stmt: LOAD DATA INFILE SSS INTO TABLE ID  */
#line 1833 "yacc_sql.y"
    {
      char *tmp_file_name = common::substr((yyvsp[-3].string), 1, strlen((yyvsp[-3].string)) - 2);
      
      (yyval.sql_node) = new ParsedSqlNode(SCF_LOAD_DATA);
      (yyval.sql_node)->load_data.relation_name = (yyvsp[0].string);
      (yyval.sql_node)->load_data.file_name = tmp_file_name;
      free((yyvsp[0].string));
      free(tmp_file_name);
    }
#line 3916 "yacc_sql.cpp"
    break;

  case 178: /* explain_stmt: EXPLAIN command_wrapper  */
#line 1846 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_EXPLAIN);
      (yyval.sql_node)->explain.sql_node = std::unique_ptr<ParsedSqlNode>((yyvsp[0].sql_node));
    }
#line 3925 "yacc_sql.cpp"
    break;

  case 179: /* set_variable_stmt: SET ID EQ value  */
#line 1854 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SET_VARIABLE);
      (yyval.sql_node)->set_variable.name  = (yyvsp[-2].string);
      (yyval.sql_node)->set_variable.value = *(yyvsp[0].value);
      free((yyvsp[-2].string));
      delete (yyvsp[0].value);
    }
#line 3937 "yacc_sql.cpp"
    break;


#line 3941 "yacc_sql.cpp"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken, &yylloc};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (&yylloc, sql_string, sql_result, scanner, yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc, sql_string, sql_result, scanner);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp, sql_string, sql_result, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, sql_string, sql_result, scanner, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, sql_string, sql_result, scanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp, sql_string, sql_result, scanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 1866 "yacc_sql.y"

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
