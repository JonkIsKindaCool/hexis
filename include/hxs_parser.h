#pragma once
#include "hxs_ast.h"
#include "hxs_lexer.h"

typedef struct 
{
    HxsLexer *lexer;
} HxsParser;

HxsParser *init_parser(const char* src);
HxsStmt *HxsParser_parse(HxsParser *parser);
HxsStmt *HxsParser_parseMultipleStmts(HxsParser *parser, HxsTokenKind ender);
HxsStmt *HxsParser_parseStatement(HxsParser *parser);
HxsExpr *HxsParser_parsePrimitive(HxsParser *parser);

bool HxsParser_maybe(HxsParser *parser, HxsTokenKind kind);

void freeParser(HxsParser* parser);