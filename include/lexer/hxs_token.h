#pragma once

#include "core/hxs_arena.h"
#include "errno.h"
#include "stdbool.h"
#include "stdlib.h"
#include "string.h"
#include <stdio.h>
#include "stdint.h"

typedef enum
{
    HXS_OP_ADD, // +
    HXS_OP_SUB, // -
    HXS_OP_MUL, // *
    HXS_OP_DIV, // /
    HXS_OP_MOD, // %

    HXS_OP_ASSIGN,     // =
    HXS_OP_ADD_ASSIGN, // +=
    HXS_OP_SUB_ASSIGN, // -=
    HXS_OP_MUL_ASSIGN, // *=
    HXS_OP_DIV_ASSIGN, // /=
    HXS_OP_MOD_ASSIGN, // %=

    HXS_OP_EQ,  // ==
    HXS_OP_NEQ, // !=
    HXS_OP_LT,  // 
    HXS_OP_LE,  // <=
    HXS_OP_GT,  // >
    HXS_OP_GE,  // >=

    HXS_OP_AND, // &&
    HXS_OP_OR,  // ||
    HXS_OP_NOT, // !

    HXS_OP_BIT_AND, // &
    HXS_OP_BIT_OR,  // |
    HXS_OP_BIT_NOT, // ~
    HXS_OP_BIT_XOR, // ^

    HXS_OP_SPREAD,       // ...
    HXS_OP_SPREAD_ASSIGN, // ..=
    HXS_OP_NULLISH,      // ??
    HXS_OP_NULL_OPT,     // .=
} HxsOpKind;

typedef enum
{
    // Primitives
    IDENTIFIER_TOKEN,
    INT_TOKEN,
    FLOAT_TOKEN,
    DOUBLE_QUOTE_STRING_TOKEN,
    SINGLE_QUOTE_STRING_TOKEN,

    // Keywords
    FUNCTION_TOKEN,
    NEW_TOKEN,
    IF_TOKEN,
    ELSE_TOKEN,
    VAR_TOKEN,
    CONST_TOKEN,
    TRUE_TOKEN,
    FALSE_TOKEN,
    WHILE_TOKEN,
    FOR_TOKEN,
    SWITCH_TOKEN,
    CASE_TOKEN,
    TRY_TOKEN,
    CATCH_TOKEN,
    CLASS_TOKEN,
    RETURN_TOKEN,

    OP_TOKEN,

    // Special characters
    COLON_TOKEN,     // :
    SEMICOLON_TOKEN, // ;
    DOT_TOKEN,       // .
    COMMA_TOKEN,     // ,

    LEFTPAREN_TOKEN,         // (
    RIGHTPAREN_TOKEN,        // )
    LEFTBRACKET_TOKEN,       // [
    RIGHTBRACKET_TOKEN,      // ]
    LEFTCURLYBRACKET_TOKEN,  // {
    RIGHTCURLYBRACKET_TOKEN, // }

    QUESTION_TOKEN, // ?

    EOF_TOKEN,
} HxsTokenKind;

typedef union
{
    int64_t  int_val;
    double   float_val;
    char    *str_val;
    HxsOpKind op;
} HxsTokenValue;

typedef struct
{
    HxsTokenKind  kind;
    HxsTokenValue value;

    size_t line;
    size_t start;
    size_t end;
} HxsToken;

HxsToken *create_base_token(HxsArena *arena, HxsTokenKind kind, size_t line, size_t start, size_t end);
HxsToken *create_op_token(HxsArena *arena, HxsOpKind op, size_t line, size_t start, size_t end);
HxsToken *make_int_token(HxsArena *arena, int64_t value, size_t line, size_t start, size_t end);
HxsToken *make_float_token(HxsArena *arena, double value, size_t line, size_t start, size_t end);
HxsToken *make_identifier_token(HxsArena *arena, char *value, size_t line, size_t start, size_t end);
HxsToken *make_string_token(HxsArena *arena, char *value, bool single, size_t line, size_t start, size_t end);

const char *token_kind_name(HxsTokenKind kind);
const char *op_kind_name(HxsOpKind op);
char *Hxs_token_to_string(HxsToken *token);