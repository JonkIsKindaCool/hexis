#pragma once

#include "ast/hxs_ast.h"

typedef struct
{
    HxsLexer *lexer;
    HxsArena *arena;

    jmp_buf error_jmp;
    char error_msg[512];
    bool has_error;
} HxsParser;

HxsExpr* Hxs_parseMultiple(HxsParser* parser, HxsTokenKind ender);

bool Hxs_parser_peek(HxsParser* parser);
bool Hxs_parser_advance(HxsParser* parser);
bool Hxs_parser_expect(HxsParser* parser, HxsTokenKind expect, const char* err);
bool Hxs_parser_maybe(HxsParser* parser, HxsTokenKind kind);
void Hxs_parser_throw(HxsParser *parser, const char *fmt, ...);