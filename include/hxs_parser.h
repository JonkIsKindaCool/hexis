#pragma once

#include "hxs_ast.h"

typedef struct
{
    HxsLexer *lexer;

    jmp_buf error_jmp;
    char error_msg[512];
    bool has_error;
} HxsParser;
