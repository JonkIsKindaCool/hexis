#pragma once

#include "stdbool.h"
#include "stdlib.h"
#include "stdint.h"
#include "hxs_utils.h"
#include <stdio.h>

typedef struct HxsExpr HxsExpr;
typedef struct HxsStmt HxsStmt;
typedef struct HxsType HxsType;

typedef enum
{
    INT_EXPR,
    FLOAT_EXPR,
    IDENTIFIER_EXPR,
    SINGLE_STRING_EXPR,
    DOUBLE_STRING_EXPR,
    BOOLEAN_EXPR,

    NEW_EXPR,
    BINARY_OP
} HXS_EXPR_KIND;

typedef enum
{
    BOP_ADD,
    BOP_SUB,
    BOP_MULT,
    BOP_DIV,
    BOP_MODULO,

    BOP_EQUAL,
    BOP_NOT_EQUAL,
    BOP_LESS,
    BOP_GREATER,
    BOP_LESS_EQUAL,
    BOP_GREATER_EQUAL,

    BOP_LAND,
    BOP_LOR,

    BOP_AND,
    BOP_OR,
    BOP_XOR,

    BOP_ASSIGN,
    BOP_ADD_ASSIGN,
    BOP_MINUS_ASSIGN,
    BOP_MUL_ASSIGN,
    BOP_DIV_ASSIGN,
    BOP_MODULO_ASSIGN,

    BOP_SPREAD,
    BOP_SPREAD_EQUAL,
    BOP_NULLISH,
    BOP_NULL

    
} Binary_Kind;

struct HxsExpr
{
    HXS_EXPR_KIND kind;
    union
    {
        struct
        {
            int64_t value;
        } int_p;

        struct
        {
            double value;
        } float_p;

        struct
        {
            char *value;
        } string_p;

        struct
        {
            bool value;
        } bool_p;

        struct
        {
            HxsExpr *left;
            HxsExpr *right;
            Binary_Kind op;
        } binary_op;
    };
};

HxsExpr *make_base_expr(HXS_EXPR_KIND kind);

HxsExpr *make_int_literal(int64_t value);
HxsExpr *make_float_literal(double value);
HxsExpr *make_bool_literal(bool value);
HxsExpr *make_string_literal(char *value, bool single);
HxsExpr *make_identifier_literal(char *value);
HxsExpr *make_binop(HxsExpr *left, HxsExpr *right, Binary_Kind op);

void free_expr(HxsExpr *expr);
StringBuffer* print_expr(HxsExpr *stmt, int spaces);

typedef enum
{
    FUNCTION_STMT,
    IF_STMT,
    WHILE_STMT,
    SWITCH_STMT,
    FOR_STMT,
    BLOCK_STMT,
    EXPR_STMT,
    VAR_STMT,
} HXS_STMT_KIND;

struct HxsStmt
{
    HXS_STMT_KIND kind;
    union
    {
        struct
        {
            HxsExpr *expression;
        } expr;

        struct
        {
            HxsStmt **body;
            int size;
        } body;

        struct
        {
            bool constant;
            HxsExpr *value;
            char *name;
            HxsType *type;
        } var;
        
    };
};

typedef enum {
    TYPE_BASIC,
    TYPE_FUNCTION,
    TYPE_ANON_OBJECT
} HxsTypeKind;

struct HxsType{
    HxsTypeKind kind;
    union
    {
        struct {
            char *name;
            int size;
            HxsType** generics;
        } basic;
    };
    
};

HxsStmt *make_base_stmt(HXS_STMT_KIND kind);
HxsStmt *make_expr_stmt(HxsExpr *value);
HxsStmt *make_var_stmt(char* name, bool constant, HxsExpr *value, HxsType *type);
void free_stmt(HxsStmt *stmt);
StringBuffer* print_stmt(HxsStmt *stmt, int spaces);

void free_type(HxsType *type);
StringBuffer* print_type(HxsType *type, int spaces);