#pragma once
#include "hxs_ast.h"
#include "hxs_lexer.h"

typedef struct
{
    HxsLexer *lexer;
} HxsParser;

HxsParser *init_parser(const char *src);
HxsStmt *HxsParser_parse(HxsParser *parser);
HxsStmt *HxsParser_parseMultipleStmts(HxsParser *parser, HxsTokenKind ender);
HxsStmt *HxsParser_parseStatement(HxsParser *parser);

HxsExpr *HxsParser_parseSpread(HxsParser* parser); // .. ..=
HxsExpr *HxsParser_parseAssignment(HxsParser *parser); // = += -= *= /= %= 

HxsExpr *HxsParser_parseNullish(HxsParser *parser); // ??

HxsExpr *HxsParser_parseLogicalOr(HxsParser *parser);  // ||
HxsExpr *HxsParser_parseLogicalAnd(HxsParser *parser); // &&

HxsExpr *HxsParser_parseBitwiseOr(HxsParser *parser);  // |
HxsExpr *HxsParser_parseBitwiseXor(HxsParser *parser); // ^
HxsExpr *HxsParser_parseBitwiseAnd(HxsParser *parser); // &

HxsExpr *HxsParser_parseEquality(HxsParser *parser);   // == !=
HxsExpr *HxsParser_parseRelational(HxsParser *parser); // < > <= >=

HxsExpr *HxsParser_parseAdditive(HxsParser *parser);       // + -
HxsExpr *HxsParser_parseMultiplicative(HxsParser *parser); // * / %

HxsExpr *HxsParser_parseNullConditional(HxsParser *parser); // ?.

HxsExpr *HxsParser_parsePrimitive(HxsParser *parser);

bool HxsParser_maybe(HxsParser *parser, HxsTokenKind kind);

void freeParser(HxsParser *parser);