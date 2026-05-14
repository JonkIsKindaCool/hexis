#include "hxs_parser.h"
#include "hxs_lexer.h"

HxsParser *init_parser(const char *src)
{
    HxsParser *parser = malloc(sizeof(HxsParser));
    parser->lexer = make_lexer(src);

    return parser;
}
HxsStmt *HxsParser_parse(HxsParser *parser)
{
    HxsStmt *block = HxsParser_parseMultipleStmts(parser, EOF_TOKEN);

    return block;
}
HxsStmt *HxsParser_parseMultipleStmts(HxsParser *parser, HxsTokenKind ender)
{
    HxsStmt *block = make_base_stmt(BLOCK_STMT);
    block->body.body = malloc(0);

    int size = 0;

    while (true)
    {
        HxsToken *token = get_token(parser->lexer, false);

        if (token->kind == ender)
        {
            freeToken(token);
            break;
        }

        freeToken(token);

        size++;
        HxsStmt **new = realloc(block->body.body, sizeof(HxsStmt *) * size);

        if (!new)
        {
            return block;
        }

        block->body.body = new;
        block->body.body[size - 1] = HxsParser_parseStatement(parser);
    }

    block->body.size = size;
    return block;
}
HxsStmt *HxsParser_parseStatement(HxsParser *parser)
{
    HxsExpr *expr = HxsParser_parsePrimitive(parser);

    return make_expr_stmt(expr);
}
HxsExpr *HxsParser_parsePrimitive(HxsParser *parser)
{
    HxsToken *token = get_token(parser->lexer, true);
    HxsExpr *expr = NULL;
    switch (token->kind)
    {
    case INT_TOKEN:
        expr = make_int_literal(token->value.int_val);
        break;

    default:
        break;
    }

    freeToken(token);
    return expr;
}

bool HxsParser_maybe(HxsParser *parser, HxsTokenKind kind)
{
    bool val = false;
    HxsToken *tok = get_token(parser->lexer, false);

    if (tok->kind == kind)
    {
        val = true;
        freeToken(get_token(parser->lexer, true));
    }

    freeToken(tok);
    return val;
}

void freeParser(HxsParser *parser)
{
    free_lexer(parser->lexer);
    free(parser);
}