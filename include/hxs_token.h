#pragma once

#include "errno.h"
#include "stdbool.h"

#include "stdlib.h"
#include "string.h"
#include <stdio.h>
#include "stdint.h"

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

    // Operator Tokens
    ADD_TOKEN,      // +
    MINUS_TOKEN,    // -
    MULTIPLY_TOKEN, // *
    DIVIDE_TOKEN,   // /
    MODULO_TOKEN,   // %

    EQUAL_TOKEN,         // ==
    NOT_EQUAL_TOKEN,     // !=
    LESS_TOKEN,          // <
    GREATER_TOKEN,       // >
    LESS_EQUAL_TOKEN,    // <=
    GREATER_EQUAL_TOKEN, // >=

    LOGICAL_AND_TOKEN, // &&
    LOGICAL_OR_TOKEN,  // ||
    LOGICAL_NOT_TOKEN, // !

    AND_BIT_TOKEN, // &
    OR_BIT_TOKEN,  // |
    XOR_BIT_TOKEN, // ^

    ASSIGN_TOKEN,          // =
    ADD_ASSIGN_TOKEN,      // +=
    MINUS_ASSIGN_TOKEN,    // -=
    MULTIPLY_ASSIGN_TOKEN, // *=
    DIVIDE_ASSIGN_TOKEN,   // /=
    MODULO_ASSIGN_TOKEN,   // %=

    SPREAD_TOKEN,        // ...
    SPREAD_EQUAL_TOKEN,  // ..=
    NULLISH_TOKEN,       // ??
    NULL_OPTIONAL_TOKEN, // .=

    // SPECIAL CHARACTERS
    COLON_TOKEN,            // :
    SEMICOLON_TOKEN,        // ;
    DOT_TOKEN,              // .
    COMMA_TOKEN,            // ,

    LEFTPAREN_TOKEN,        // (
    RIGHTPAREN_TOKEN,       // )
    LEFTBRACKET_TOKEN,      // [
    RIGHTBRACKET_TOKEN,     // ]
    LEFTCURLYBRACKET_TOKEN, // {
    RIGHTCURLYBRACKET_TOKEN,// }

    QUESTION_TOKEN, //?

    EOF_TOKEN,

} HxsTokenKind;

typedef union
{
    int64_t  int_val;
    double   float_val;
    char    *str_val;
} HxsTokenValue;

typedef struct
{
    HxsTokenKind  kind;
    HxsTokenValue value;

    size_t line;
    size_t start;
    size_t end;
} HxsToken;

HxsToken *create_base_token(HxsTokenKind kind, size_t line, size_t start, size_t end);
HxsToken *make_int_token(int64_t value, size_t line, size_t start, size_t end);
HxsToken *make_float_token(double value, size_t line, size_t start, size_t end);
HxsToken *make_identifier_token(const char *value, size_t line, size_t start, size_t end);
HxsToken *make_string_token(const char *value, bool single, size_t line, size_t start, size_t end);
void      freeToken(HxsToken *token);

const char *token_kind_name(HxsTokenKind kind);
char       *token_to_string(HxsToken *token);