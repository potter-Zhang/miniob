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
    field_extract(a_expr->right().get(), rel_attr_node);
    return;
  }
  if (expr->type() == ExprType::FUNCTION) {
    FunctionExpr *f_expr = static_cast<FunctionExpr *>(expr);
    field_extract(f_expr->expr(), rel_attr_node);
    return;
  }
}


#line 185 "yacc_sql.cpp"

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
#define YYLAST   489

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  84
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  59
/* YYNRULES -- Number of rules.  */
#define YYNRULES  180
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  376

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
       0,   300,   300,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   332,   338,   343,   349,   355,   361,
     367,   374,   380,   388,   399,   417,   438,   448,   464,   476,
     479,   492,   496,   500,   506,   514,   522,   532,   535,   536,
     537,   538,   541,   557,   560,   572,   585,   588,   599,   603,
     607,   615,   622,   632,   644,   659,   678,   681,   692,   703,
     748,   788,   795,   807,   810,   820,   830,   843,   853,   864,
     877,   880,   883,   886,   889,   893,   897,   900,   903,   908,
     911,   917,   926,   975,   987,   992,   999,  1005,  1011,  1018,
    1029,  1035,  1041,  1050,  1056,  1063,  1119,  1122,  1137,  1140,
    1155,  1158,  1168,  1181,  1197,  1206,  1218,  1233,  1236,  1239,
    1242,  1245,  1251,  1254,  1260,  1263,  1269,  1272,  1281,  1293,
    1302,  1376,  1382,  1396,  1402,  1416,  1420,  1425,  1436,  1447,
    1458,  1469,  1484,  1499,  1514,  1530,  1539,  1549,  1560,  1576,
    1600,  1616,  1634,  1635,  1636,  1637,  1638,  1639,  1640,  1641,
    1646,  1649,  1662,  1665,  1678,  1681,  1696,  1699,  1710,  1723,
    1736,  1750,  1766,  1785,  1796,  1810,  1827,  1840,  1848,  1858,
    1859
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

#define YYPACT_NINF (-295)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-93)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     379,     9,   145,   273,   199,   -57,    56,  -295,    38,    60,
      -6,  -295,  -295,  -295,  -295,  -295,    48,    86,   379,   159,
     142,  -295,  -295,  -295,  -295,  -295,  -295,  -295,  -295,  -295,
    -295,  -295,  -295,  -295,  -295,  -295,  -295,  -295,  -295,  -295,
    -295,  -295,    84,    85,   156,    91,    96,   273,  -295,  -295,
    -295,  -295,  -295,  -295,   158,   163,   169,  -295,  -295,  -295,
     161,  -295,   273,  -295,  -295,    97,  -295,   171,  -295,   160,
     162,  -295,  -295,   114,   118,   166,   157,   168,  -295,  -295,
    -295,  -295,     4,   174,   120,  -295,   175,    27,   273,   273,
     273,   -47,  -295,   273,   137,   196,   273,   273,   273,   273,
    -295,   -44,   140,   187,   191,   143,   -35,   144,   149,   218,
     154,   200,   167,  -295,    39,    33,   123,  -295,  -295,   153,
     196,  -295,   -54,   -54,  -295,  -295,     6,   219,    -1,   191,
     191,   226,   236,  -295,   205,  -295,   225,   258,   237,    78,
     199,  -295,   241,   201,  -295,  -295,  -295,   194,   213,   215,
     196,  -295,  -295,  -295,   216,  -295,   222,   238,   240,     3,
    -295,  -295,   242,   242,   -35,   276,   189,  -295,  -295,   252,
     302,   278,   323,   381,  -295,   286,    72,   321,  -295,  -295,
    -295,  -295,   311,   149,   312,  -295,  -295,  -295,  -295,   262,
     335,   322,   354,   196,  -295,   363,    -5,    11,     3,  -295,
    -295,   308,   266,   266,   367,   226,  -295,   369,   396,   218,
    -295,  -295,   398,   332,  -295,  -295,  -295,  -295,  -295,  -295,
    -295,  -295,   283,  -295,  -295,   401,   333,   236,   218,   410,
     343,   356,   237,  -295,    92,   355,  -295,  -295,  -295,  -295,
     361,   412,  -295,   236,   362,     0,  -295,  -295,  -295,   407,
     423,   359,   331,   331,   -35,   425,   276,   380,   218,   426,
      65,   430,   189,   224,    65,   431,  -295,   432,   372,   191,
    -295,  -295,   433,  -295,  -295,   434,   412,   412,  -295,   399,
      47,   236,  -295,   376,   308,  -295,   380,   380,   438,  -295,
     422,   132,  -295,  -295,   367,  -295,  -295,   132,   439,  -295,
     367,   440,    65,   441,   367,   442,    65,   410,   420,  -295,
    -295,  -295,   444,  -295,  -295,   236,  -295,   399,  -295,   423,
     324,   -35,    23,   359,     5,   445,  -295,  -295,  -295,   447,
    -295,   367,   448,  -295,   449,  -295,   367,   450,   191,   304,
    -295,   399,  -295,  -295,  -295,  -295,    24,   451,  -295,  -295,
    -295,   132,  -295,  -295,   452,  -295,  -295,   453,  -295,  -295,
     218,   410,  -295,  -295,   386,  -295,   445,  -295,  -295,   454,
    -295,   455,  -295,   410,  -295,  -295
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,    26,     0,     0,
       0,    27,    28,    29,    25,    24,     0,     0,     0,     0,
     179,    23,    22,    15,    16,    17,    18,     9,    10,    11,
      12,    13,    14,     8,     5,     7,     6,     4,     3,    19,
      20,    21,     0,     0,     0,     0,     0,     0,   117,   118,
     119,   120,   121,    62,     0,     0,     0,    58,    59,    60,
      94,    61,     0,    90,    71,    73,    91,     0,    93,    72,
       0,    32,    31,     0,     0,     0,     0,     0,   177,     1,
     180,     2,     0,     0,     0,    30,     0,     0,     0,     0,
       0,     0,    89,     0,     0,    73,     0,     0,     0,     0,
      77,     0,     0,     0,   122,     0,     0,     0,     0,     0,
       0,     0,     0,    84,     0,     0,     0,    95,    96,    73,
      73,    78,    80,    81,    82,    83,     0,     0,   110,   122,
     122,     0,   126,    63,     0,   178,     0,     0,    39,    41,
       0,    38,     0,     0,    36,    85,    86,     0,     0,     0,
      73,    74,    79,    97,     0,    98,     0,     0,     0,   110,
     114,   173,   160,   160,     0,    53,     0,   135,   136,     0,
       0,    90,     0,    91,   123,   127,     0,     0,    48,    49,
      50,    51,    46,     0,     0,    42,    43,    44,    92,     0,
       0,     0,     0,    73,    75,     0,   110,   166,   110,   115,
     174,     0,   124,   124,    56,     0,    52,     0,     0,     0,
     131,   133,     0,     0,   152,   153,   154,   155,   156,   157,
     158,   159,     0,   132,   134,     0,     0,   126,     0,    66,
       0,     0,    39,    37,     0,     0,    87,    88,    76,    99,
       0,   110,   111,   126,     0,   166,   167,   116,   175,   164,
     162,     0,   108,   108,     0,     0,    53,     0,     0,     0,
       0,     0,     0,   149,     0,     0,   128,     0,     0,   122,
     176,    47,     0,    40,    33,     0,   110,   110,   112,   166,
     166,   126,   168,     0,     0,   161,     0,     0,     0,   125,
     129,     0,    69,    70,    56,    55,    54,     0,     0,   145,
      56,     0,     0,     0,    56,     0,     0,    66,     0,    64,
      45,    35,     0,   113,   170,   126,   169,   166,   165,   162,
       0,     0,     0,     0,   100,   106,    57,   148,   146,     0,
     138,    56,     0,   147,     0,   137,    56,     0,   122,     0,
      34,   166,   171,   163,   151,   150,     0,     0,   130,   102,
     101,     0,   109,   142,     0,   140,   141,     0,   139,    65,
       0,    66,   172,   103,     0,   104,   106,   144,   143,     0,
      67,     0,   107,    66,   105,    68
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -295,  -295,   456,  -295,  -295,  -295,  -295,  -295,  -295,  -295,
    -295,  -295,  -295,  -295,  -295,  -295,   243,   293,  -295,  -295,
    -295,  -295,   221,   274,  -275,   -94,  -295,  -295,  -294,  -106,
    -295,   -88,    -2,    -3,  -295,  -126,   127,   164,   115,   227,
    -150,  -295,  -243,  -125,   279,  -217,   165,  -295,  -295,  -158,
     320,   170,   202,  -148,  -295,  -295,  -295,  -295,  -295
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,   184,   138,   139,   272,
     182,    34,   206,   165,   255,    63,    35,    36,   269,    37,
      38,   100,    64,   172,    70,    66,   325,   287,   352,   292,
     160,   129,    67,   133,   252,   174,   289,   175,   290,   222,
     202,   285,   250,   161,   130,    39,    40,    41,    81
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      65,    65,    69,   141,   162,   163,   173,   121,   288,   199,
     266,   200,   135,   338,   156,    42,   349,    43,   156,   326,
      71,   108,   156,    53,   153,   329,   279,    98,    99,   334,
     117,   151,   152,   126,   118,   154,   281,   127,   171,    57,
      58,    59,   363,    61,    87,   113,   242,   243,   247,   246,
     248,   146,   147,   364,   157,   157,   354,   145,   157,    92,
     207,   357,   194,    72,   317,   240,   157,   370,    73,   158,
     204,    75,   241,   350,   109,   140,   159,   288,    44,   375,
     288,   244,   229,   315,   275,   114,   115,   116,   245,   228,
     119,   278,    74,   122,   123,   124,   125,   282,   341,   297,
     346,   173,   157,   259,   347,   238,    96,    97,    98,    99,
     274,   156,    96,    97,    98,    99,    93,   173,    96,    97,
      98,    99,   267,    53,    77,    76,   312,   313,   320,   321,
      53,   314,   316,   171,   185,   186,   187,    65,   188,    57,
      58,    59,   148,    61,   309,    80,    57,    58,    59,   171,
      61,    45,   298,    46,   301,   173,   303,   286,   305,    79,
     294,    82,    83,    87,    84,   324,   300,    94,    85,   342,
     304,   327,    93,    86,    95,    88,    96,    97,    98,    99,
      89,    48,    49,    50,    51,    52,    90,   171,   101,   173,
      91,   103,   -92,   362,   102,   104,   332,   111,   106,   140,
     337,   105,    96,    97,    98,    99,    47,   107,   331,    60,
     110,   112,   336,   359,   120,    93,    47,   128,   131,   263,
     134,   171,   136,   149,   132,   324,   137,   345,   140,   286,
     150,   142,    96,    97,    98,    99,   143,   155,    48,    49,
      50,    51,    52,   164,   144,   361,   176,    53,    48,    49,
      50,    51,    52,   166,   369,   177,   183,    53,   189,    87,
      54,    55,    56,    57,    58,    59,    60,    61,   191,    62,
      54,    55,    56,    57,    58,    59,    60,    61,   190,    62,
      68,   178,   179,   180,   181,    48,    49,    50,    51,    52,
      47,   192,   193,   195,    53,   205,   201,   167,   168,   196,
     262,   169,   170,    96,    97,    98,    99,    54,    55,    56,
      57,    58,    59,    60,    61,   197,    62,   198,   208,   209,
     227,   360,    48,    49,    50,    51,    52,   230,   231,   251,
     233,    53,    48,    49,    50,    51,    52,   210,   211,   234,
     236,    53,   212,   213,    54,    55,    56,    57,    58,    59,
      60,    61,   235,    62,    54,    55,    56,    57,    58,    59,
      60,    61,    53,    62,   214,   215,   216,   217,   218,   219,
     220,   221,   237,    48,    49,    50,    51,    52,    57,    58,
      59,   239,    61,     1,     2,   249,   254,   257,     3,     4,
       5,     6,     7,     8,     9,    10,   261,   265,   291,    11,
      12,    13,    96,    97,    98,    99,    14,    15,    48,    49,
      50,    51,    52,   258,    16,   260,    17,    53,   264,    18,
     270,   214,   215,   216,   217,   218,   219,   220,   221,   268,
     271,   156,   276,    57,    58,    59,   283,    61,   277,   280,
     223,   224,   284,   295,   299,   225,   226,   302,   306,   308,
     307,   310,   311,   318,   157,   322,   323,   328,   330,   333,
     335,   339,   340,   371,   351,   353,   355,   356,   358,   365,
     367,   368,   373,   374,    78,   273,   232,   296,   366,   256,
     293,   372,   253,   203,   344,     0,   319,     0,   348,   343
};

static const yytype_int16 yycheck[] =
{
       3,     4,     4,   109,   129,   130,   132,    95,   251,   159,
     227,   159,   106,   307,    19,     6,    11,     8,    19,   294,
      77,    17,    19,    58,    18,   300,   243,    81,    82,   304,
      77,   119,   120,    77,    81,    29,    36,    81,   132,    74,
      75,    76,    18,    78,    47,    18,   196,    36,   198,   197,
     198,    18,    19,    29,    55,    55,   331,    18,    55,    62,
     166,   336,   150,     7,   281,    70,    55,   361,    30,    70,
     164,    77,    77,    68,    70,    10,    77,   320,    69,   373,
     323,    70,   176,    36,   234,    88,    89,    90,    77,    17,
      93,   241,    32,    96,    97,    98,    99,   245,   315,   257,
      77,   227,    55,   209,    81,   193,    79,    80,    81,    82,
      18,    19,    79,    80,    81,    82,    19,   243,    79,    80,
      81,    82,   228,    58,    38,    77,   276,   277,   286,   287,
      58,   279,   280,   227,    56,    57,    58,   140,   140,    74,
      75,    76,    19,    78,   269,     3,    74,    75,    76,   243,
      78,     6,   258,     8,   260,   281,   262,   251,   264,     0,
     254,    77,    77,   166,     8,   291,   260,    70,    77,   317,
     264,   297,    19,    77,    77,    17,    79,    80,    81,    82,
      17,    49,    50,    51,    52,    53,    17,   281,    17,   315,
      29,    77,    32,   341,    32,    77,   302,    77,    41,    10,
     306,    35,    79,    80,    81,    82,    17,    39,   302,    77,
      36,    36,   306,   338,    77,    19,    17,    77,    31,   222,
      77,   315,    78,    70,    33,   351,    77,   321,    10,   323,
      77,    77,    79,    80,    81,    82,    36,    18,    49,    50,
      51,    52,    53,    17,    77,   339,    41,    58,    49,    50,
      51,    52,    53,    17,   360,    30,    19,    58,    17,   262,
      71,    72,    73,    74,    75,    76,    77,    78,    74,    80,
      71,    72,    73,    74,    75,    76,    77,    78,    77,    80,
      81,    23,    24,    25,    26,    49,    50,    51,    52,    53,
      17,    78,    77,    77,    58,    19,    54,    61,    62,    77,
      17,    65,    66,    79,    80,    81,    82,    71,    72,    73,
      74,    75,    76,    77,    78,    77,    80,    77,    66,    17,
      34,    17,    49,    50,    51,    52,    53,     6,    17,    63,
      18,    58,    49,    50,    51,    52,    53,    59,    60,    77,
      18,    58,    64,    65,    71,    72,    73,    74,    75,    76,
      77,    78,    17,    80,    71,    72,    73,    74,    75,    76,
      77,    78,    58,    80,    41,    42,    43,    44,    45,    46,
      47,    48,    18,    49,    50,    51,    52,    53,    74,    75,
      76,    18,    78,     4,     5,    77,    19,    18,     9,    10,
      11,    12,    13,    14,    15,    16,    64,    64,    67,    20,
      21,    22,    79,    80,    81,    82,    27,    28,    49,    50,
      51,    52,    53,    17,    35,    17,    37,    58,    17,    40,
      77,    41,    42,    43,    44,    45,    46,    47,    48,    19,
      74,    19,    77,    74,    75,    76,    29,    78,    77,    77,
      59,    60,    19,    18,    18,    64,    65,    17,    17,    77,
      18,    18,    18,    77,    55,    17,    34,    18,    18,    18,
      18,    41,    18,    77,    19,    18,    18,    18,    18,    18,
      18,    18,    18,    18,    18,   232,   183,   256,   351,   205,
     253,   366,   203,   163,   320,    -1,   284,    -1,   323,   319
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
      17,    29,   117,    19,    70,    77,    79,    80,    81,    82,
     115,    17,    32,    77,    77,    35,    41,    39,    17,    70,
      36,    77,    36,    18,   117,   117,   117,    77,    81,   117,
      77,   115,   117,   117,   117,   117,    77,    81,    77,   125,
     138,    31,    33,   127,    77,   109,    78,    77,   101,   102,
      10,   113,    77,    36,    77,    18,    18,    19,    19,    70,
      77,   115,   115,    18,    29,    18,    19,    55,    70,    77,
     124,   137,   127,   127,    17,   107,    17,    61,    62,    65,
      66,   109,   117,   119,   129,   131,    41,    30,    23,    24,
      25,    26,   104,    19,   100,    56,    57,    58,   116,    17,
      77,    74,    78,    77,   115,    77,    77,    77,    77,   124,
     137,    54,   134,   134,   109,    19,   106,   113,    66,    17,
      59,    60,    64,    65,    41,    42,    43,    44,    45,    46,
      47,    48,   133,    59,    60,    64,    65,    34,    17,   109,
       6,    17,   101,    18,    77,    17,    18,    18,   115,    18,
      70,    77,   124,    36,    70,    77,   137,   124,   137,    77,
     136,    63,   128,   128,    19,   108,   107,    18,    17,   113,
      17,    64,    17,   117,    17,    64,   129,   113,    19,   112,
      77,    74,   103,   100,    18,   124,    77,    77,   124,   129,
      77,    36,   137,    29,    19,   135,   109,   121,   126,   130,
     132,    67,   123,   123,   109,    18,   106,   133,   113,    18,
     109,   113,    17,   113,   109,   113,    17,    18,    77,   127,
      18,    18,   124,   124,   137,    36,   137,   129,    77,   136,
     133,   133,    17,    34,   119,   120,   108,   119,    18,   108,
      18,   109,   113,    18,   108,    18,   109,   113,   112,    41,
      18,   129,   137,   135,   121,   109,    77,    81,   130,    11,
      68,    19,   122,    18,   108,    18,    18,   108,    18,   127,
      17,   109,   137,    18,    29,    18,   120,    18,    18,   113,
     112,    77,   122,    18,    18,   112
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
     117,   117,   118,   118,   119,   119,   119,   119,   119,   119,
     120,   120,   120,   121,   121,   121,   122,   122,   123,   123,
     124,   124,   124,   124,   125,   125,   125,   126,   126,   126,
     126,   126,   127,   127,   128,   128,   129,   129,   129,   130,
     130,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     131,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     132,   132,   133,   133,   133,   133,   133,   133,   133,   133,
     134,   134,   135,   135,   136,   136,   137,   137,   137,   137,
     137,   137,   137,   138,   138,   138,   139,   140,   141,   142,
     142
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
       1,     1,     1,     1,     1,     3,     3,     4,     4,     6,
       1,     2,     2,     4,     4,     6,     0,     3,     0,     3,
       0,     3,     4,     5,     2,     3,     4,     1,     1,     1,
       1,     1,     0,     2,     0,     2,     0,     1,     3,     1,
       3,     2,     2,     2,     2,     1,     1,     5,     5,     6,
       6,     6,     6,     7,     7,     4,     5,     5,     5,     3,
       3,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       0,     3,     0,     3,     1,     3,     0,     3,     4,     5,
       5,     6,     7,     2,     3,     4,     7,     2,     4,     0,
       1
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
#line 301 "yacc_sql.y"
  {
    std::unique_ptr<ParsedSqlNode> sql_node = std::unique_ptr<ParsedSqlNode>((yyvsp[-1].sql_node));
    sql_result->add_sql_node(std::move(sql_node));
  }
#line 2006 "yacc_sql.cpp"
    break;

  case 24: /* exit_stmt: EXIT  */
#line 332 "yacc_sql.y"
         {
      (void)yynerrs;  // 这么写为了消除yynerrs未使用的告警。如果你有更好的方法欢迎提PR
      (yyval.sql_node) = new ParsedSqlNode(SCF_EXIT);
    }
#line 2015 "yacc_sql.cpp"
    break;

  case 25: /* help_stmt: HELP  */
#line 338 "yacc_sql.y"
         {
      (yyval.sql_node) = new ParsedSqlNode(SCF_HELP);
    }
#line 2023 "yacc_sql.cpp"
    break;

  case 26: /* sync_stmt: SYNC  */
#line 343 "yacc_sql.y"
         {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SYNC);
    }
#line 2031 "yacc_sql.cpp"
    break;

  case 27: /* begin_stmt: TRX_BEGIN  */
#line 349 "yacc_sql.y"
               {
      (yyval.sql_node) = new ParsedSqlNode(SCF_BEGIN);
    }
#line 2039 "yacc_sql.cpp"
    break;

  case 28: /* commit_stmt: TRX_COMMIT  */
#line 355 "yacc_sql.y"
               {
      (yyval.sql_node) = new ParsedSqlNode(SCF_COMMIT);
    }
#line 2047 "yacc_sql.cpp"
    break;

  case 29: /* rollback_stmt: TRX_ROLLBACK  */
#line 361 "yacc_sql.y"
                  {
      (yyval.sql_node) = new ParsedSqlNode(SCF_ROLLBACK);
    }
#line 2055 "yacc_sql.cpp"
    break;

  case 30: /* drop_table_stmt: DROP TABLE ID  */
#line 367 "yacc_sql.y"
                  {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DROP_TABLE);
      (yyval.sql_node)->drop_table.relation_name = (yyvsp[0].string);
      free((yyvsp[0].string));
    }
#line 2065 "yacc_sql.cpp"
    break;

  case 31: /* show_tables_stmt: SHOW TABLES  */
#line 374 "yacc_sql.y"
                {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SHOW_TABLES);
    }
#line 2073 "yacc_sql.cpp"
    break;

  case 32: /* desc_table_stmt: DESC ID  */
#line 380 "yacc_sql.y"
             {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DESC_TABLE);
      (yyval.sql_node)->desc_table.relation_name = (yyvsp[0].string);
      free((yyvsp[0].string));
    }
#line 2083 "yacc_sql.cpp"
    break;

  case 33: /* create_index_stmt: CREATE INDEX ID ON ID LBRACE ID RBRACE  */
#line 389 "yacc_sql.y"
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
#line 2098 "yacc_sql.cpp"
    break;

  case 34: /* create_index_stmt: CREATE UNIQUE INDEX ID ON ID LBRACE ID rel_list RBRACE  */
#line 400 "yacc_sql.y"
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
#line 2117 "yacc_sql.cpp"
    break;

  case 35: /* create_multi_index_stmt: CREATE INDEX ID ON ID LBRACE ID rel_list RBRACE  */
#line 418 "yacc_sql.y"
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
#line 2139 "yacc_sql.cpp"
    break;

  case 36: /* drop_index_stmt: DROP INDEX ID ON ID  */
#line 439 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DROP_INDEX);
      (yyval.sql_node)->drop_index.index_name = (yyvsp[-2].string);
      (yyval.sql_node)->drop_index.relation_name = (yyvsp[0].string);
      free((yyvsp[-2].string));
      free((yyvsp[0].string));
    }
#line 2151 "yacc_sql.cpp"
    break;

  case 37: /* create_table_stmt: CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE  */
#line 449 "yacc_sql.y"
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
#line 2171 "yacc_sql.cpp"
    break;

  case 38: /* create_table_stmt: CREATE TABLE ID AS select_stmt  */
#line 465 "yacc_sql.y"
    {
        (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_TABLE_SELECT);
        CreateTableSelectSqlNode &create_table_select = (yyval.sql_node)->create_table_select;
        create_table_select.relation_name = (yyvsp[-2].string);
        free((yyvsp[-2].string));

        create_table_select.select = &(yyvsp[0].sql_node)->selection;
    }
#line 2184 "yacc_sql.cpp"
    break;

  case 39: /* attr_def_list: %empty  */
#line 476 "yacc_sql.y"
    {
      (yyval.attr_infos) = nullptr;
    }
#line 2192 "yacc_sql.cpp"
    break;

  case 40: /* attr_def_list: COMMA attr_def attr_def_list  */
#line 480 "yacc_sql.y"
    {
      if ((yyvsp[0].attr_infos) != nullptr) {
        (yyval.attr_infos) = (yyvsp[0].attr_infos);
      } else {
        (yyval.attr_infos) = new std::vector<AttrInfoSqlNode>;
      }
      (yyval.attr_infos)->emplace_back(*(yyvsp[-1].attr_info));
      delete (yyvsp[-1].attr_info);
    }
#line 2206 "yacc_sql.cpp"
    break;

  case 41: /* attr_def: simple_attr_def  */
#line 493 "yacc_sql.y"
    {
      (yyval.attr_info) = (yyvsp[0].attr_info);
    }
#line 2214 "yacc_sql.cpp"
    break;

  case 42: /* attr_def: simple_attr_def NOTNULL  */
#line 497 "yacc_sql.y"
    {
      (yyval.attr_info) = (yyvsp[-1].attr_info);
    }
#line 2222 "yacc_sql.cpp"
    break;

  case 43: /* attr_def: simple_attr_def NULLABLE  */
#line 501 "yacc_sql.y"
    {
      (yyval.attr_info) = (yyvsp[-1].attr_info);
      (yyval.attr_info)->nullable = true;
      (yyval.attr_info)->length ++;
    }
#line 2232 "yacc_sql.cpp"
    break;

  case 44: /* attr_def: simple_attr_def NULLVALUE  */
#line 507 "yacc_sql.y"
    {
      (yyval.attr_info) = (yyvsp[-1].attr_info);
      (yyval.attr_info)->nullable = true;
      (yyval.attr_info)->length ++;
    }
#line 2242 "yacc_sql.cpp"
    break;

  case 45: /* simple_attr_def: ID type LBRACE number RBRACE  */
#line 515 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-3].number);
      (yyval.attr_info)->name = (yyvsp[-4].string);
      (yyval.attr_info)->length = (yyvsp[-1].number);
      free((yyvsp[-4].string));
    }
#line 2254 "yacc_sql.cpp"
    break;

  case 46: /* simple_attr_def: ID type  */
#line 523 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[0].number);
      (yyval.attr_info)->name = (yyvsp[-1].string);
      (yyval.attr_info)->length = 4;
      free((yyvsp[-1].string));
    }
#line 2266 "yacc_sql.cpp"
    break;

  case 47: /* number: NUMBER  */
#line 532 "yacc_sql.y"
           {(yyval.number) = (yyvsp[0].number);}
#line 2272 "yacc_sql.cpp"
    break;

  case 48: /* type: INT_T  */
#line 535 "yacc_sql.y"
               { (yyval.number)=INTS; }
#line 2278 "yacc_sql.cpp"
    break;

  case 49: /* type: STRING_T  */
#line 536 "yacc_sql.y"
               { (yyval.number)=CHARS; }
#line 2284 "yacc_sql.cpp"
    break;

  case 50: /* type: FLOAT_T  */
#line 537 "yacc_sql.y"
               { (yyval.number)=FLOATS; }
#line 2290 "yacc_sql.cpp"
    break;

  case 51: /* type: DATE_T  */
#line 538 "yacc_sql.y"
               { (yyval.number)=DATES; }
#line 2296 "yacc_sql.cpp"
    break;

  case 52: /* insert_stmt: INSERT INTO ID VALUES value_row value_rows  */
#line 542 "yacc_sql.y"
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
#line 2312 "yacc_sql.cpp"
    break;

  case 53: /* value_rows: %empty  */
#line 557 "yacc_sql.y"
    {
      (yyval.value_rows) = nullptr;
    }
#line 2320 "yacc_sql.cpp"
    break;

  case 54: /* value_rows: COMMA value_row value_rows  */
#line 560 "yacc_sql.y"
                                { 
      if ((yyvsp[0].value_rows) != nullptr) {
        (yyval.value_rows) = (yyvsp[0].value_rows);
      } else {
        (yyval.value_rows) = new std::vector<Value>;
      }
      std::copy((yyvsp[-1].value_row)->begin(), (yyvsp[-1].value_row)->end(), std::back_inserter(*(yyval.value_rows)));
      delete (yyvsp[-1].value_row);
    }
#line 2334 "yacc_sql.cpp"
    break;

  case 55: /* value_row: LBRACE value value_list RBRACE  */
#line 573 "yacc_sql.y"
    {
      (yyval.value_row) = new std::vector<Value>;
      if ((yyvsp[-1].value_list) != nullptr) {
        (yyval.value_row)->swap(*(yyvsp[-1].value_list));
      }
      (yyval.value_row)->emplace_back(*(yyvsp[-2].value));
      delete (yyvsp[-2].value);
    }
#line 2347 "yacc_sql.cpp"
    break;

  case 56: /* value_list: %empty  */
#line 585 "yacc_sql.y"
    {
      (yyval.value_list) = nullptr;
    }
#line 2355 "yacc_sql.cpp"
    break;

  case 57: /* value_list: COMMA value value_list  */
#line 588 "yacc_sql.y"
                              { 
      if ((yyvsp[0].value_list) != nullptr) {
        (yyval.value_list) = (yyvsp[0].value_list);
      } else {
        (yyval.value_list) = new std::vector<Value>;
      }
      (yyval.value_list)->emplace_back(*(yyvsp[-1].value));
      delete (yyvsp[-1].value);
    }
#line 2369 "yacc_sql.cpp"
    break;

  case 58: /* value: NUMBER  */
#line 599 "yacc_sql.y"
           {
      (yyval.value) = new Value((int)(yyvsp[0].number));
      (yyloc) = (yylsp[0]);
    }
#line 2378 "yacc_sql.cpp"
    break;

  case 59: /* value: FLOAT  */
#line 603 "yacc_sql.y"
           {
      (yyval.value) = new Value((float)(yyvsp[0].floats));
      (yyloc) = (yylsp[0]);
    }
#line 2387 "yacc_sql.cpp"
    break;

  case 60: /* value: DATE  */
#line 607 "yacc_sql.y"
          {
      char *tmp = common::substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
      (yyval.value) = new Value((Date)tmp);
      free(tmp);
      if (!(yyval.value)->get_date().check()){
        YYABORT;
      }
    }
#line 2400 "yacc_sql.cpp"
    break;

  case 61: /* value: SSS  */
#line 615 "yacc_sql.y"
         {
      
        char *tmp = common::substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
        (yyval.value) = new Value(tmp);
        free(tmp);
      
    }
#line 2412 "yacc_sql.cpp"
    break;

  case 62: /* value: NULLVALUE  */
#line 622 "yacc_sql.y"
              {
      (yyval.value) = new Value();
      (yyval.value)->set_type(AttrType::NULLTYPE);
      (yyval.value)->set_nullable(true);
      (yyval.value)->set_is_null(true);
    }
#line 2423 "yacc_sql.cpp"
    break;

  case 63: /* delete_stmt: DELETE FROM ID where  */
#line 633 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DELETE);
      (yyval.sql_node)->deletion.relation_name = (yyvsp[-1].string);
      if ((yyvsp[0].condition_list) != nullptr) {
        (yyval.sql_node)->deletion.conditions.swap(*(yyvsp[0].condition_list));
        delete (yyvsp[0].condition_list);
      }
      free((yyvsp[-1].string));
    }
#line 2437 "yacc_sql.cpp"
    break;

  case 64: /* update_stmt: UPDATE ID SET ID EQ value set_list where  */
#line 645 "yacc_sql.y"
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
#line 2456 "yacc_sql.cpp"
    break;

  case 65: /* update_stmt: UPDATE ID SET ID EQ LBRACE select_stmt RBRACE set_list where  */
#line 660 "yacc_sql.y"
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
#line 2476 "yacc_sql.cpp"
    break;

  case 66: /* set_list: %empty  */
#line 678 "yacc_sql.y"
    {
      (yyval.set_list) = nullptr;
    }
#line 2484 "yacc_sql.cpp"
    break;

  case 67: /* set_list: COMMA ID EQ value set_list  */
#line 682 "yacc_sql.y"
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
#line 2499 "yacc_sql.cpp"
    break;

  case 68: /* set_list: COMMA ID EQ LBRACE select_stmt RBRACE set_list  */
#line 693 "yacc_sql.y"
    {
      if ((yyvsp[0].set_list) != nullptr) {
        (yyval.set_list) = (yyvsp[0].set_list);
      } else {
        (yyval.set_list) = new std::vector<AttrValuePair>;
      }
      (yyval.set_list)->emplace_back(AttrValuePair{(yyvsp[-5].string), Value(), &(yyvsp[-2].sql_node)->selection});
      delete (yyvsp[-5].string);
    }
#line 2513 "yacc_sql.cpp"
    break;

  case 69: /* select_stmt: SELECT select_attr FROM tables where group_by_columns having order_list  */
#line 704 "yacc_sql.y"
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
#line 2562 "yacc_sql.cpp"
    break;

  case 70: /* select_stmt: SELECT select_attr FROM complete_inner_join_node where group_by_columns having order_list  */
#line 749 "yacc_sql.y"
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
#line 2604 "yacc_sql.cpp"
    break;

  case 71: /* calc_stmt: CALC expression_list  */
#line 789 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CALC);
      std::reverse((yyvsp[0].expression_list)->begin(), (yyvsp[0].expression_list)->end());
      (yyval.sql_node)->calc.expressions.swap(*(yyvsp[0].expression_list));
      delete (yyvsp[0].expression_list);
    }
#line 2615 "yacc_sql.cpp"
    break;

  case 72: /* calc_stmt: SELECT expression_list  */
#line 796 "yacc_sql.y"
    {
      //std::cout << "calc" << std::endl;
      (yyval.sql_node) = new ParsedSqlNode(SCF_CALC);
      std::reverse((yyvsp[0].expression_list)->begin(), (yyvsp[0].expression_list)->end());
      (yyval.sql_node)->calc.expressions.swap(*(yyvsp[0].expression_list));
      delete (yyvsp[0].expression_list);
    }
#line 2627 "yacc_sql.cpp"
    break;

  case 73: /* part_expression_list: %empty  */
#line 807 "yacc_sql.y"
    {
      (yyval.expression_list) = nullptr;
    }
#line 2635 "yacc_sql.cpp"
    break;

  case 74: /* part_expression_list: COMMA expression part_expression_list  */
#line 811 "yacc_sql.y"
    {
      if ((yyvsp[0].expression_list) != nullptr) {
        (yyval.expression_list) = (yyvsp[0].expression_list);
      } else {
        (yyval.expression_list) = new std::vector<Expression *>;
      }

      (yyval.expression_list)->emplace_back((yyvsp[-1].expression));
    }
#line 2649 "yacc_sql.cpp"
    break;

  case 75: /* part_expression_list: COMMA expression ID part_expression_list  */
#line 821 "yacc_sql.y"
    {
      if ((yyvsp[0].expression_list) != nullptr) {
        (yyval.expression_list) = (yyvsp[0].expression_list);
      } else {
        (yyval.expression_list) = new std::vector<Expression *>;
      }
      (yyvsp[-2].expression)->set_name((yyvsp[-1].string));
      (yyval.expression_list)->emplace_back((yyvsp[-2].expression));
    }
#line 2663 "yacc_sql.cpp"
    break;

  case 76: /* part_expression_list: COMMA expression AS ID part_expression_list  */
#line 831 "yacc_sql.y"
    {
      if ((yyvsp[0].expression_list) != nullptr) {
        (yyval.expression_list) = (yyvsp[0].expression_list);
      } else {
        (yyval.expression_list) = new std::vector<Expression *>;
      }
      (yyvsp[-3].expression)->set_name((yyvsp[-1].string));
      (yyval.expression_list)->emplace_back((yyvsp[-3].expression));
    }
#line 2677 "yacc_sql.cpp"
    break;

  case 77: /* expression_list: expression part_expression_list  */
#line 844 "yacc_sql.y"
    {
      if ((yyvsp[0].expression_list) != nullptr) {
        (yyval.expression_list) = (yyvsp[0].expression_list);
      } else {
        (yyval.expression_list) = new std::vector<Expression*>;
      }

      (yyval.expression_list)->emplace_back((yyvsp[-1].expression));
    }
#line 2691 "yacc_sql.cpp"
    break;

  case 78: /* expression_list: expression ID part_expression_list  */
#line 854 "yacc_sql.y"
    {
      if ((yyvsp[0].expression_list) != nullptr) {
        (yyval.expression_list) = (yyvsp[0].expression_list);
      } else {
        (yyval.expression_list) = new std::vector<Expression*>;
      }

      (yyvsp[-2].expression)->set_name((yyvsp[-1].string));
      (yyval.expression_list)->emplace_back((yyvsp[-2].expression));
    }
#line 2706 "yacc_sql.cpp"
    break;

  case 79: /* expression_list: expression AS ID part_expression_list  */
#line 865 "yacc_sql.y"
    {
      if ((yyvsp[0].expression_list) != nullptr) {
        (yyval.expression_list) = (yyvsp[0].expression_list);
      } else {
        (yyval.expression_list) = new std::vector<Expression*>;
      }

      (yyvsp[-3].expression)->set_name((yyvsp[-1].string));
      (yyval.expression_list)->emplace_back((yyvsp[-3].expression));
    }
#line 2721 "yacc_sql.cpp"
    break;

  case 80: /* expression: expression '+' expression  */
#line 877 "yacc_sql.y"
                              {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::ADD, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2729 "yacc_sql.cpp"
    break;

  case 81: /* expression: expression '-' expression  */
#line 880 "yacc_sql.y"
                                {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::SUB, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2737 "yacc_sql.cpp"
    break;

  case 82: /* expression: expression '*' expression  */
#line 883 "yacc_sql.y"
                                {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::MUL, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2745 "yacc_sql.cpp"
    break;

  case 83: /* expression: expression '/' expression  */
#line 886 "yacc_sql.y"
                                {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::DIV, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2753 "yacc_sql.cpp"
    break;

  case 84: /* expression: LBRACE expression RBRACE  */
#line 889 "yacc_sql.y"
                               {
      (yyval.expression) = (yyvsp[-1].expression);
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
    }
#line 2762 "yacc_sql.cpp"
    break;

  case 85: /* expression: LENGTH LBRACE expression RBRACE  */
#line 893 "yacc_sql.y"
                                      {
      (yyval.expression) = create_function_expression(FunctionExpr::Type::LENGTH, (yyvsp[-1].expression), sql_string, &(yyloc));
      //std::cout << "length" << std::endl;
    }
#line 2771 "yacc_sql.cpp"
    break;

  case 86: /* expression: ROUND LBRACE expression RBRACE  */
#line 897 "yacc_sql.y"
                                     {
      (yyval.expression) = create_function_expression(FunctionExpr::Type::ROUND, (yyvsp[-1].expression), 0, sql_string, &(yyloc));
    }
#line 2779 "yacc_sql.cpp"
    break;

  case 87: /* expression: ROUND LBRACE expression COMMA NUMBER RBRACE  */
#line 900 "yacc_sql.y"
                                                  {
      (yyval.expression) = create_function_expression(FunctionExpr::Type::ROUND, (yyvsp[-3].expression), (yyvsp[-1].number), sql_string, &(yyloc));
    }
#line 2787 "yacc_sql.cpp"
    break;

  case 88: /* expression: DATE_FORMAT LBRACE expression COMMA SSS RBRACE  */
#line 903 "yacc_sql.y"
                                                     {
      char *tmp = common::substr((yyvsp[-1].string),1,strlen((yyvsp[-1].string))-2);
      (yyval.expression) = create_function_expression(FunctionExpr::Type::DATE_FORMAT, (yyvsp[-3].expression), tmp, sql_string, &(yyloc));
      free(tmp);
    }
#line 2797 "yacc_sql.cpp"
    break;

  case 89: /* expression: '-' expression  */
#line 908 "yacc_sql.y"
                                  {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::NEGATIVE, (yyvsp[0].expression), nullptr, sql_string, &(yyloc));
    }
#line 2805 "yacc_sql.cpp"
    break;

  case 90: /* expression: value  */
#line 911 "yacc_sql.y"
            {
      //std::cout << "value" << std::endl;
      (yyval.expression) = new ValueExpr(*(yyvsp[0].value));
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
      delete (yyvsp[0].value);
    }
#line 2816 "yacc_sql.cpp"
    break;

  case 91: /* expression: rel_attr  */
#line 917 "yacc_sql.y"
               {
      (yyval.expression) = new FieldExpr(*(yyvsp[0].rel_attr));
      (yyval.expression)->set_name("");
      //$$->set_name(token_name(sql_string, &@$));
      delete (yyvsp[0].rel_attr);
    }
#line 2827 "yacc_sql.cpp"
    break;

  case 92: /* select_attr: expression_list  */
#line 927 "yacc_sql.y"
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
#line 2844 "yacc_sql.cpp"
    break;

  case 93: /* select_attr: '*'  */
#line 975 "yacc_sql.y"
          {
      (yyval.rel_attr_list) = new std::vector<RelAttrSqlNode>;
      RelAttrSqlNode attr;
      attr.relation_name  = "";
      attr.attribute_name = "*";
      attr.expr = nullptr;
      (yyval.rel_attr_list)->emplace_back(attr);
      
    }
#line 2858 "yacc_sql.cpp"
    break;

  case 94: /* rel_attr: ID  */
#line 987 "yacc_sql.y"
       {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->attribute_name = (yyvsp[0].string);
      free((yyvsp[0].string));
    }
#line 2868 "yacc_sql.cpp"
    break;

  case 95: /* rel_attr: ID DOT ID  */
#line 992 "yacc_sql.y"
                {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->relation_name  = (yyvsp[-2].string);
      (yyval.rel_attr)->attribute_name = (yyvsp[0].string);
      free((yyvsp[-2].string));
      free((yyvsp[0].string));
    }
#line 2880 "yacc_sql.cpp"
    break;

  case 96: /* rel_attr: ID DOT '*'  */
#line 999 "yacc_sql.y"
                 {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->relation_name  = (yyvsp[-2].string);
      (yyval.rel_attr)->attribute_name = '*';
      free((yyvsp[-2].string));
    }
#line 2891 "yacc_sql.cpp"
    break;

  case 97: /* rel_attr: func LBRACE ID RBRACE  */
#line 1005 "yacc_sql.y"
                            {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->attribute_name = (yyvsp[-1].string);
      (yyval.rel_attr)->func = (yyvsp[-3].func);
      free((yyvsp[-1].string));
    }
#line 2902 "yacc_sql.cpp"
    break;

  case 98: /* rel_attr: func LBRACE '*' RBRACE  */
#line 1011 "yacc_sql.y"
                             {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->attribute_name = "*";
      if ((yyvsp[-3].func) != AggregationFunc::COUNTFUN)
        YYABORT;
      (yyval.rel_attr)->func = (yyvsp[-3].func);
    }
#line 2914 "yacc_sql.cpp"
    break;

  case 99: /* rel_attr: func LBRACE ID DOT ID RBRACE  */
#line 1018 "yacc_sql.y"
                                   {
      (yyval.rel_attr) = new RelAttrSqlNode;;
      (yyval.rel_attr)->relation_name = (yyvsp[-3].string);
      (yyval.rel_attr)->attribute_name = (yyvsp[-1].string);
      (yyval.rel_attr)->func = (yyvsp[-5].func);
      free((yyvsp[-3].string));
      free((yyvsp[-1].string));
    }
#line 2927 "yacc_sql.cpp"
    break;

  case 100: /* order_attr: rel_attr  */
#line 1029 "yacc_sql.y"
             {
      (yyval.pair) = new std::pair<RelAttrSqlNode, bool>;
      (yyval.pair)->first = *(yyvsp[0].rel_attr);
      (yyval.pair)->second = true;
      delete (yyvsp[0].rel_attr);
    }
#line 2938 "yacc_sql.cpp"
    break;

  case 101: /* order_attr: rel_attr ASC  */
#line 1035 "yacc_sql.y"
                   {
      (yyval.pair) = new std::pair<RelAttrSqlNode, bool>;
      (yyval.pair)->first = *(yyvsp[-1].rel_attr);
      (yyval.pair)->second = true;
      delete (yyvsp[-1].rel_attr);
    }
#line 2949 "yacc_sql.cpp"
    break;

  case 102: /* order_attr: rel_attr DESC  */
#line 1041 "yacc_sql.y"
                    {
      (yyval.pair) = new std::pair<RelAttrSqlNode, bool>;
      (yyval.pair)->first = *(yyvsp[-1].rel_attr);
      (yyval.pair)->second = false;
      delete (yyvsp[-1].rel_attr);
    }
#line 2960 "yacc_sql.cpp"
    break;

  case 103: /* agg_attr: func LBRACE ID RBRACE  */
#line 1050 "yacc_sql.y"
                          {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->attribute_name = (yyvsp[-1].string);
      (yyval.rel_attr)->func = (yyvsp[-3].func);
      free((yyvsp[-1].string));
    }
#line 2971 "yacc_sql.cpp"
    break;

  case 104: /* agg_attr: func LBRACE '*' RBRACE  */
#line 1056 "yacc_sql.y"
                             {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->attribute_name = "*";
      if ((yyvsp[-3].func) != AggregationFunc::COUNTFUN)
        YYABORT;
      (yyval.rel_attr)->func = (yyvsp[-3].func);
    }
#line 2983 "yacc_sql.cpp"
    break;

  case 105: /* agg_attr: func LBRACE ID DOT ID RBRACE  */
#line 1063 "yacc_sql.y"
                                   {
      (yyval.rel_attr) = new RelAttrSqlNode;;
      (yyval.rel_attr)->relation_name = (yyvsp[-3].string);
      (yyval.rel_attr)->attribute_name = (yyvsp[-1].string);
      (yyval.rel_attr)->func = (yyvsp[-5].func);
      free((yyvsp[-3].string));
      free((yyvsp[-1].string));
    }
#line 2996 "yacc_sql.cpp"
    break;

  case 106: /* unordered_list: %empty  */
#line 1119 "yacc_sql.y"
    {
      (yyval.unordered_list) = nullptr;
    }
#line 3004 "yacc_sql.cpp"
    break;

  case 107: /* unordered_list: COMMA order_attr unordered_list  */
#line 1123 "yacc_sql.y"
    {
      if ((yyvsp[0].unordered_list) != nullptr) {
        (yyval.unordered_list) = (yyvsp[0].unordered_list);
      } else {
        (yyval.unordered_list) = new std::vector<std::pair<RelAttrSqlNode, bool>>;
      }
      
      (yyval.unordered_list)->emplace_back(*(yyvsp[-1].pair));
      delete (yyvsp[-1].pair);
    }
#line 3019 "yacc_sql.cpp"
    break;

  case 108: /* order_list: %empty  */
#line 1137 "yacc_sql.y"
    {
      (yyval.unordered_list) = nullptr;
    }
#line 3027 "yacc_sql.cpp"
    break;

  case 109: /* order_list: ORDERBY order_attr unordered_list  */
#line 1141 "yacc_sql.y"
    {
      if ((yyvsp[0].unordered_list) == nullptr) {
        (yyval.unordered_list) = new std::vector<std::pair<RelAttrSqlNode, bool>>;
      } else {
        (yyval.unordered_list) = (yyvsp[0].unordered_list);
      }

      (yyval.unordered_list)->emplace_back(*(yyvsp[-1].pair));
      delete (yyvsp[-1].pair);
    }
#line 3042 "yacc_sql.cpp"
    break;

  case 110: /* rel_list: %empty  */
#line 1155 "yacc_sql.y"
    {
      (yyval.relation_list) = nullptr;
    }
#line 3050 "yacc_sql.cpp"
    break;

  case 111: /* rel_list: COMMA ID rel_list  */
#line 1158 "yacc_sql.y"
                        {
      if ((yyvsp[0].relation_list) != nullptr) {
        (yyval.relation_list) = (yyvsp[0].relation_list);
      } else {
        (yyval.relation_list) = new std::vector<std::string>;
      }

      (yyval.relation_list)->push_back((yyvsp[-1].string));
      free((yyvsp[-1].string));
    }
#line 3065 "yacc_sql.cpp"
    break;

  case 112: /* rel_list: COMMA ID ID rel_list  */
#line 1168 "yacc_sql.y"
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
#line 3083 "yacc_sql.cpp"
    break;

  case 113: /* rel_list: COMMA ID AS ID rel_list  */
#line 1181 "yacc_sql.y"
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
#line 3101 "yacc_sql.cpp"
    break;

  case 114: /* tables: ID rel_list  */
#line 1197 "yacc_sql.y"
                {
      if ((yyvsp[0].relation_list) != nullptr) {
        (yyval.relation_list) = (yyvsp[0].relation_list);
      } else {
        (yyval.relation_list) = new std::vector<std::string>;
      }
      (yyval.relation_list)->push_back((yyvsp[-1].string));
      free((yyvsp[-1].string));
    }
#line 3115 "yacc_sql.cpp"
    break;

  case 115: /* tables: ID ID rel_list  */
#line 1206 "yacc_sql.y"
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
#line 3132 "yacc_sql.cpp"
    break;

  case 116: /* tables: ID AS ID rel_list  */
#line 1218 "yacc_sql.y"
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
#line 3149 "yacc_sql.cpp"
    break;

  case 117: /* func: MAX  */
#line 1233 "yacc_sql.y"
        {
      (yyval.func) = AggregationFunc::MAXFUN;
    }
#line 3157 "yacc_sql.cpp"
    break;

  case 118: /* func: MIN  */
#line 1236 "yacc_sql.y"
          {
      (yyval.func) = AggregationFunc::MINFUN;
    }
#line 3165 "yacc_sql.cpp"
    break;

  case 119: /* func: COUNT  */
#line 1239 "yacc_sql.y"
            {
      (yyval.func) = AggregationFunc::COUNTFUN;
    }
#line 3173 "yacc_sql.cpp"
    break;

  case 120: /* func: AVG  */
#line 1242 "yacc_sql.y"
          {
      (yyval.func) = AggregationFunc::AVGFUN;
    }
#line 3181 "yacc_sql.cpp"
    break;

  case 121: /* func: SUM  */
#line 1245 "yacc_sql.y"
          {
      (yyval.func) = AggregationFunc::SUMFUN;
    }
#line 3189 "yacc_sql.cpp"
    break;

  case 122: /* where: %empty  */
#line 1251 "yacc_sql.y"
    {
      (yyval.condition_list) = nullptr;
    }
#line 3197 "yacc_sql.cpp"
    break;

  case 123: /* where: WHERE condition_list  */
#line 1254 "yacc_sql.y"
                           {
      (yyval.condition_list) = (yyvsp[0].condition_list);  
    }
#line 3205 "yacc_sql.cpp"
    break;

  case 124: /* having: %empty  */
#line 1260 "yacc_sql.y"
    {
      (yyval.condition_list) = nullptr;
    }
#line 3213 "yacc_sql.cpp"
    break;

  case 125: /* having: HAVING agg_condition_list  */
#line 1263 "yacc_sql.y"
                                {
      (yyval.condition_list) = (yyvsp[0].condition_list);
    }
#line 3221 "yacc_sql.cpp"
    break;

  case 126: /* condition_list: %empty  */
#line 1269 "yacc_sql.y"
    {
      (yyval.condition_list) = nullptr;
    }
#line 3229 "yacc_sql.cpp"
    break;

  case 127: /* condition_list: condition  */
#line 1272 "yacc_sql.y"
                {
      if ((yyvsp[0].condition) != nullptr) {
        (yyval.condition_list) = new std::vector<ConditionSqlNode>;
        (yyval.condition_list)->emplace_back(*(yyvsp[0].condition));
        delete (yyvsp[0].condition);
      }
      else
        (yyval.condition_list) = nullptr;
    }
#line 3243 "yacc_sql.cpp"
    break;

  case 128: /* condition_list: condition AND condition_list  */
#line 1281 "yacc_sql.y"
                                   {
      (yyval.condition_list) = (yyvsp[0].condition_list);
      if ((yyvsp[-2].condition) != nullptr){
        if ((yyval.condition_list) == nullptr)
          (yyval.condition_list) = new std::vector<ConditionSqlNode>;
        (yyval.condition_list)->emplace_back(*(yyvsp[-2].condition));
        delete (yyvsp[-2].condition);
      }
    }
#line 3257 "yacc_sql.cpp"
    break;

  case 129: /* agg_condition_list: agg_condition  */
#line 1293 "yacc_sql.y"
                  {
      if ((yyvsp[0].condition) != nullptr) {
        (yyval.condition_list) = new std::vector<ConditionSqlNode>;
        (yyval.condition_list)->emplace_back(*(yyvsp[0].condition));
        delete (yyvsp[0].condition);
      }
      else
        (yyval.condition_list) = nullptr;
    }
#line 3271 "yacc_sql.cpp"
    break;

  case 130: /* agg_condition_list: agg_condition AND agg_condition_list  */
#line 1302 "yacc_sql.y"
                                           {
      (yyval.condition_list) = (yyvsp[0].condition_list);
      if ((yyvsp[-2].condition) != nullptr){
        if ((yyval.condition_list) == nullptr)
          (yyval.condition_list) = new std::vector<ConditionSqlNode>;
        (yyval.condition_list)->emplace_back(*(yyvsp[-2].condition));
        delete (yyvsp[-2].condition);
      }
    }
#line 3285 "yacc_sql.cpp"
    break;

  case 131: /* condition: value ISNULL  */
#line 1377 "yacc_sql.y"
    {
      (yyval.condition) = always_false();

      delete (yyvsp[-1].value);
    }
#line 3295 "yacc_sql.cpp"
    break;

  case 132: /* condition: rel_attr ISNULL  */
#line 1383 "yacc_sql.y"
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
#line 3313 "yacc_sql.cpp"
    break;

  case 133: /* condition: value ISNOTNULL  */
#line 1397 "yacc_sql.y"
    {
      (yyval.condition) = nullptr;

      delete (yyvsp[-1].value);
    }
#line 3323 "yacc_sql.cpp"
    break;

  case 134: /* condition: rel_attr ISNOTNULL  */
#line 1403 "yacc_sql.y"
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
#line 3341 "yacc_sql.cpp"
    break;

  case 135: /* condition: NULLISNULL  */
#line 1417 "yacc_sql.y"
    {
      (yyval.condition) = nullptr;
    }
#line 3349 "yacc_sql.cpp"
    break;

  case 136: /* condition: NULLISNOTNULL  */
#line 1421 "yacc_sql.y"
    {
      (yyval.condition) = always_false();
    }
#line 3357 "yacc_sql.cpp"
    break;

  case 137: /* condition: rel_attr IN LBRACE select_stmt RBRACE  */
#line 1426 "yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 1;
      (yyval.condition)->left_attr = *(yyvsp[-4].rel_attr);
      (yyval.condition)->right_is_attr = 2;
      (yyval.condition)->right_select = &((yyvsp[-1].sql_node)->selection);
      (yyval.condition)->comp = CompOp::IN_OP;

      delete (yyvsp[-4].rel_attr);
    }
#line 3372 "yacc_sql.cpp"
    break;

  case 138: /* condition: value IN LBRACE select_stmt RBRACE  */
#line 1437 "yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 0;
      (yyval.condition)->left_value = *(yyvsp[-4].value);
      (yyval.condition)->right_is_attr = 2;
      (yyval.condition)->right_select = &((yyvsp[-1].sql_node)->selection);
      (yyval.condition)->comp = CompOp::IN_OP;

      delete (yyvsp[-4].value);
    }
#line 3387 "yacc_sql.cpp"
    break;

  case 139: /* condition: rel_attr NOT IN LBRACE select_stmt RBRACE  */
#line 1448 "yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 1;
      (yyval.condition)->left_attr = *(yyvsp[-5].rel_attr);
      (yyval.condition)->right_is_attr = 2;
      (yyval.condition)->right_select = &((yyvsp[-1].sql_node)->selection);
      (yyval.condition)->comp = CompOp::NOT_IN_OP;

      delete (yyvsp[-5].rel_attr);
    }
#line 3402 "yacc_sql.cpp"
    break;

  case 140: /* condition: value NOT IN LBRACE select_stmt RBRACE  */
#line 1459 "yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 0;
      (yyval.condition)->left_value = *(yyvsp[-5].value);
      (yyval.condition)->right_is_attr = 2;
      (yyval.condition)->right_select = &((yyvsp[-1].sql_node)->selection);
      (yyval.condition)->comp = CompOp::NOT_IN_OP;

      delete (yyvsp[-5].value);
    }
#line 3417 "yacc_sql.cpp"
    break;

  case 141: /* condition: rel_attr IN LBRACE value value_list RBRACE  */
#line 1470 "yacc_sql.y"
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
#line 3436 "yacc_sql.cpp"
    break;

  case 142: /* condition: value IN LBRACE value value_list RBRACE  */
#line 1485 "yacc_sql.y"
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
#line 3455 "yacc_sql.cpp"
    break;

  case 143: /* condition: rel_attr NOT IN LBRACE value value_list RBRACE  */
#line 1500 "yacc_sql.y"
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
#line 3474 "yacc_sql.cpp"
    break;

  case 144: /* condition: value NOT IN LBRACE value value_list RBRACE  */
#line 1515 "yacc_sql.y"
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
#line 3493 "yacc_sql.cpp"
    break;

  case 145: /* condition: EXISTS LBRACE select_stmt RBRACE  */
#line 1531 "yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 2;
      (yyval.condition)->right_is_attr = 2;
      (yyval.condition)->right_select = &((yyvsp[-1].sql_node)->selection);
      (yyval.condition)->comp = CompOp::EXISTS_OP;

    }
#line 3506 "yacc_sql.cpp"
    break;

  case 146: /* condition: NOT EXISTS LBRACE select_stmt RBRACE  */
#line 1540 "yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 2;
      (yyval.condition)->right_is_attr = 2;
      (yyval.condition)->right_select = &((yyvsp[-1].sql_node)->selection);
      (yyval.condition)->comp = CompOp::NOT_EXISTS_OP;

    }
#line 3519 "yacc_sql.cpp"
    break;

  case 147: /* condition: expression comp_op LBRACE select_stmt RBRACE  */
#line 1550 "yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 4;
      (yyval.condition)->left_expr = (yyvsp[-4].expression);
      (yyval.condition)->right_is_attr = 2;
      (yyval.condition)->right_select = &((yyvsp[-1].sql_node)->selection);
      (yyval.condition)->comp = (yyvsp[-3].comp);

      
    }
#line 3534 "yacc_sql.cpp"
    break;

  case 148: /* condition: LBRACE select_stmt RBRACE comp_op rel_attr  */
#line 1561 "yacc_sql.y"
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
#line 3553 "yacc_sql.cpp"
    break;

  case 149: /* condition: expression comp_op expression  */
#line 1577 "yacc_sql.y"
    {
      (yyval.condition) = new ConditionSqlNode;
      (yyval.condition)->left_is_attr = 4;
      (yyval.condition)->left_expr = (yyvsp[-2].expression);
      (yyval.condition)->comp = (yyvsp[-1].comp);
      (yyval.condition)->right_is_attr = 4;
      (yyval.condition)->right_expr = (yyvsp[0].expression);
    
    }
#line 3567 "yacc_sql.cpp"
    break;

  case 150: /* agg_condition: agg_attr comp_op value  */
#line 1601 "yacc_sql.y"
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
#line 3587 "yacc_sql.cpp"
    break;

  case 151: /* agg_condition: value comp_op agg_attr  */
#line 1617 "yacc_sql.y"
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
#line 3607 "yacc_sql.cpp"
    break;

  case 152: /* comp_op: EQ  */
#line 1634 "yacc_sql.y"
         { (yyval.comp) = EQUAL_TO; }
#line 3613 "yacc_sql.cpp"
    break;

  case 153: /* comp_op: LT  */
#line 1635 "yacc_sql.y"
         { (yyval.comp) = LESS_THAN; }
#line 3619 "yacc_sql.cpp"
    break;

  case 154: /* comp_op: GT  */
#line 1636 "yacc_sql.y"
         { (yyval.comp) = GREAT_THAN; }
#line 3625 "yacc_sql.cpp"
    break;

  case 155: /* comp_op: LE  */
#line 1637 "yacc_sql.y"
         { (yyval.comp) = LESS_EQUAL; }
#line 3631 "yacc_sql.cpp"
    break;

  case 156: /* comp_op: GE  */
#line 1638 "yacc_sql.y"
         { (yyval.comp) = GREAT_EQUAL; }
#line 3637 "yacc_sql.cpp"
    break;

  case 157: /* comp_op: NE  */
#line 1639 "yacc_sql.y"
         { (yyval.comp) = NOT_EQUAL; }
#line 3643 "yacc_sql.cpp"
    break;

  case 158: /* comp_op: LIKE  */
#line 1640 "yacc_sql.y"
           { (yyval.comp) = LIKE_A; }
#line 3649 "yacc_sql.cpp"
    break;

  case 159: /* comp_op: NOT_LIKE  */
#line 1641 "yacc_sql.y"
               { (yyval.comp) = LIKE_NOT; }
#line 3655 "yacc_sql.cpp"
    break;

  case 160: /* group_by_columns: %empty  */
#line 1646 "yacc_sql.y"
    {
      (yyval.relation_list) = nullptr;
    }
#line 3663 "yacc_sql.cpp"
    break;

  case 161: /* group_by_columns: GROUPBY group_by_attr group_by_attr_list  */
#line 1650 "yacc_sql.y"
    {
      (yyval.relation_list) = new std::vector<std::string>;
      if ((yyvsp[0].relation_list) != nullptr){
        (yyval.relation_list)->swap(*(yyvsp[0].relation_list));
        delete (yyvsp[0].relation_list);
      }
      (yyval.relation_list)->push_back((yyvsp[-1].string));
      free((yyvsp[-1].string));
    }
#line 3677 "yacc_sql.cpp"
    break;

  case 162: /* group_by_attr_list: %empty  */
#line 1662 "yacc_sql.y"
    {
      (yyval.relation_list) = nullptr;
    }
#line 3685 "yacc_sql.cpp"
    break;

  case 163: /* group_by_attr_list: COMMA group_by_attr group_by_attr_list  */
#line 1665 "yacc_sql.y"
                                            {
      if ((yyvsp[0].relation_list) != nullptr){
        (yyval.relation_list) = (yyvsp[0].relation_list);
      } else {
        (yyval.relation_list) = new std::vector<std::string>;
      }

      (yyval.relation_list)->push_back((yyvsp[-1].string));
      free((yyvsp[-1].string));
    }
#line 3700 "yacc_sql.cpp"
    break;

  case 164: /* group_by_attr: ID  */
#line 1678 "yacc_sql.y"
       {
      (yyval.string) = (yyvsp[0].string);
    }
#line 3708 "yacc_sql.cpp"
    break;

  case 165: /* group_by_attr: ID DOT ID  */
#line 1681 "yacc_sql.y"
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
#line 3724 "yacc_sql.cpp"
    break;

  case 166: /* inner_join_node: %empty  */
#line 1696 "yacc_sql.y"
    {
      (yyval.sql_node) = nullptr;
    }
#line 3732 "yacc_sql.cpp"
    break;

  case 167: /* inner_join_node: INNERJOIN ID inner_join_node  */
#line 1700 "yacc_sql.y"
    {      
      if ((yyvsp[0].sql_node) != nullptr){
        (yyval.sql_node) = (yyvsp[0].sql_node);
      } else {
        (yyval.sql_node) = new ParsedSqlNode(SCF_SELECT);
      }

      (yyval.sql_node)->selection.relations.push_back((yyvsp[-1].string));
      free((yyvsp[-1].string));
    }
#line 3747 "yacc_sql.cpp"
    break;

  case 168: /* inner_join_node: INNERJOIN ID ID inner_join_node  */
#line 1711 "yacc_sql.y"
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
#line 3764 "yacc_sql.cpp"
    break;

  case 169: /* inner_join_node: INNERJOIN ID AS ID inner_join_node  */
#line 1724 "yacc_sql.y"
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
#line 3781 "yacc_sql.cpp"
    break;

  case 170: /* inner_join_node: INNERJOIN ID ON condition_list inner_join_node  */
#line 1737 "yacc_sql.y"
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
#line 3799 "yacc_sql.cpp"
    break;

  case 171: /* inner_join_node: INNERJOIN ID ID ON condition_list inner_join_node  */
#line 1751 "yacc_sql.y"
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
#line 3819 "yacc_sql.cpp"
    break;

  case 172: /* inner_join_node: INNERJOIN ID AS ID ON condition_list inner_join_node  */
#line 1767 "yacc_sql.y"
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
#line 3839 "yacc_sql.cpp"
    break;

  case 173: /* complete_inner_join_node: ID inner_join_node  */
#line 1786 "yacc_sql.y"
    {
      if ((yyvsp[0].sql_node) != nullptr) {
        (yyval.sql_node) = (yyvsp[0].sql_node);
      } else {
        (yyval.sql_node) = new ParsedSqlNode(SCF_SELECT);
      }

      (yyval.sql_node)->selection.relations.push_back((yyvsp[-1].string));
      free((yyvsp[-1].string));
    }
#line 3854 "yacc_sql.cpp"
    break;

  case 174: /* complete_inner_join_node: ID ID inner_join_node  */
#line 1797 "yacc_sql.y"
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
#line 3872 "yacc_sql.cpp"
    break;

  case 175: /* complete_inner_join_node: ID AS ID inner_join_node  */
#line 1811 "yacc_sql.y"
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
#line 3890 "yacc_sql.cpp"
    break;

  case 176: /* load_data_stmt: LOAD DATA INFILE SSS INTO TABLE ID  */
#line 1828 "yacc_sql.y"
    {
      char *tmp_file_name = common::substr((yyvsp[-3].string), 1, strlen((yyvsp[-3].string)) - 2);
      
      (yyval.sql_node) = new ParsedSqlNode(SCF_LOAD_DATA);
      (yyval.sql_node)->load_data.relation_name = (yyvsp[0].string);
      (yyval.sql_node)->load_data.file_name = tmp_file_name;
      free((yyvsp[0].string));
      free(tmp_file_name);
    }
#line 3904 "yacc_sql.cpp"
    break;

  case 177: /* explain_stmt: EXPLAIN command_wrapper  */
#line 1841 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_EXPLAIN);
      (yyval.sql_node)->explain.sql_node = std::unique_ptr<ParsedSqlNode>((yyvsp[0].sql_node));
    }
#line 3913 "yacc_sql.cpp"
    break;

  case 178: /* set_variable_stmt: SET ID EQ value  */
#line 1849 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SET_VARIABLE);
      (yyval.sql_node)->set_variable.name  = (yyvsp[-2].string);
      (yyval.sql_node)->set_variable.value = *(yyvsp[0].value);
      free((yyvsp[-2].string));
      delete (yyvsp[0].value);
    }
#line 3925 "yacc_sql.cpp"
    break;


#line 3929 "yacc_sql.cpp"

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

#line 1861 "yacc_sql.y"

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
