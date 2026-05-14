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

    while (!HxsParser_maybe(parser, EOF_TOKEN))
    {
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
    case FLOAT_TOKEN:
        expr = make_float_literal(token->value.float_val);
        break;
    case IDENTIFIER_TOKEN:
        expr = make_identifier_literal(token->value.str_val);
        break;
    case SINGLE_QUOTE_STRING_TOKEN:
        expr = make_string_literal(token->value.str_val, true);
        break;
    case DOUBLE_QUOTE_STRING_TOKEN:
        expr = make_string_literal(token->value.str_val, false);
        break;
    case TRUE_TOKEN:
        expr = make_bool_literal(true);
        break;
    case FALSE_TOKEN:
        expr = make_bool_literal(false);
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