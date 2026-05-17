#pragma once

#include "lexer/hxs_lexer.h"
#include "ast/hxs_ast.h"

typedef struct HxsParser
{
    HxsArena *arena;
    HxsLexer *lexer;
    HxsToken *current;
    HxsToken *peek;
    const char *filename;   
    jmp_buf error_jmp;
    char error_msg[256];
    bool has_error;
} HxsParser;

typedef struct
{
    HxsBinop op;
    int precedence;
} Hxs_BinopPrecedence;

HxsParser *Hxs_Parser_new(HxsArena *arena, HxsLexer *lexer, const char *filename);
HxsExpr *Hxs_Parser_parse_program(HxsParser *parser);
void Hxs_Parser_free(HxsParser *parser);