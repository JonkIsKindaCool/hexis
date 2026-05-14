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
    if (setjmp(parser->error_jmp) != 0)
    {
        fprintf(stderr, "[ParseError] %s\n", parser->error_msg);
        return NULL;
    }
    return HxsParser_parseMultipleStmts(parser, EOF_TOKEN);
}
HxsStmt *HxsParser_parseMultipleStmts(HxsParser *parser, HxsTokenKind ender)
{
    HxsStmt *block = make_base_stmt(BLOCK_STMT);
    block->body.body = malloc(0);
    int size = 0;

    while (!HxsParser_maybe(parser, ender))
    {
        HxsToken *peek = get_token(parser->lexer, false);
        bool is_eof = peek->kind == EOF_TOKEN;
        freeToken(peek);

        if (is_eof)
            HxsParser_throw(parser,
                            "Unexpected end of file, expected '%s'",
                            token_kind_name(ender));

        size++;
        HxsStmt **new = realloc(block->body.body, sizeof(HxsStmt *) * size);
        if (!new)
            HxsParser_throw(parser, "Out of memory in parseMultipleStmts");

        block->body.body = new;
        block->body.body[size - 1] = HxsParser_parseStatement(parser);
    }

    block->body.size = size;
    return block;
}
HxsStmt *HxsParser_parseStatement(HxsParser *parser)
{
    HxsToken *peek = get_token(parser->lexer, false);
    HxsTokenKind kind = peek->kind;
    freeToken(peek);

    switch (kind)
    {
    case EOF_TOKEN:
        HxsParser_throw(parser, "Unexpected end of file in statement");
        return NULL;

    default:
    {
        HxsExpr *expr = HxsParser_parseSpread(parser);
        if (!expr)
            HxsParser_throw(parser, "Empty or invalid expression in statement");
        return make_expr_stmt(expr);
    }
    }
}

HxsExpr *HxsParser_parseSpread(HxsParser *parser) // .. ..=
{
    HxsExpr *left = HxsParser_parseAssignment(parser);

    if (HxsParser_maybe(parser, SPREAD_TOKEN))
    {
        HxsExpr *right = HxsParser_parseAssignment(parser);
        left = make_binop(left, right, BOP_SPREAD);
    }
    else if (HxsParser_maybe(parser, SPREAD_EQUAL_TOKEN))
    {
        HxsExpr *right = HxsParser_parseAssignment(parser);
        left = make_binop(left, right, BOP_SPREAD_EQUAL);
    }

    return left;
}

HxsExpr *HxsParser_parseAssignment(HxsParser *parser)
{
    HxsExpr *left = HxsParser_parseNullish(parser);

    if (HxsParser_maybe(parser, ASSIGN_TOKEN))
    {
        HxsExpr *right = HxsParser_parseAssignment(parser);
        left = make_binop(left, right, BOP_ASSIGN);
    }
    else if (HxsParser_maybe(parser, ADD_ASSIGN_TOKEN))
    {
        HxsExpr *right = HxsParser_parseAssignment(parser);
        left = make_binop(left, right, BOP_ADD_ASSIGN);
    }
    else if (HxsParser_maybe(parser, MINUS_ASSIGN_TOKEN))
    {
        HxsExpr *right = HxsParser_parseAssignment(parser);
        left = make_binop(left, right, BOP_MINUS_ASSIGN);
    }
    else if (HxsParser_maybe(parser, MULTIPLY_ASSIGN_TOKEN))
    {
        HxsExpr *right = HxsParser_parseAssignment(parser);
        left = make_binop(left, right, BOP_MUL_ASSIGN);
    }
    else if (HxsParser_maybe(parser, DIVIDE_ASSIGN_TOKEN))
    {
        HxsExpr *right = HxsParser_parseAssignment(parser);
        left = make_binop(left, right, BOP_DIV_ASSIGN);
    }
    else if (HxsParser_maybe(parser, MODULO_ASSIGN_TOKEN))
    {
        HxsExpr *right = HxsParser_parseAssignment(parser);
        left = make_binop(left, right, BOP_MODULO_ASSIGN);
    }
    else if (HxsParser_maybe(parser, SPREAD_EQUAL_TOKEN))
    {
        HxsExpr *right = HxsParser_parseAssignment(parser);
        left = make_binop(left, right, BOP_SPREAD_EQUAL);
    }

    return left;
}

HxsExpr *HxsParser_parseNullish(HxsParser *parser) // ??
{
    HxsExpr *left = HxsParser_parseLogicalOr(parser);

    if (HxsParser_maybe(parser, NULLISH_TOKEN))
    {
        HxsExpr *right = HxsParser_parseNullish(parser);
        left = make_binop(left, right, BOP_NULLISH);
    }

    return left;
}

HxsExpr *HxsParser_parseLogicalOr(HxsParser *parser) // ||
{
    HxsExpr *left = HxsParser_parseLogicalAnd(parser);

    if (HxsParser_maybe(parser, LOGICAL_OR_TOKEN))
    {
        HxsExpr *right = HxsParser_parseLogicalOr(parser);
        left = make_binop(left, right, BOP_LOR);
    }

    return left;
}

HxsExpr *HxsParser_parseLogicalAnd(HxsParser *parser) // &&
{
    HxsExpr *left = HxsParser_parseBitwiseOr(parser);

    if (HxsParser_maybe(parser, LOGICAL_AND_TOKEN))
    {
        HxsExpr *right = HxsParser_parseLogicalAnd(parser);
        left = make_binop(left, right, BOP_LAND);
    }

    return left;
}

HxsExpr *HxsParser_parseBitwiseOr(HxsParser *parser) // |
{
    HxsExpr *left = HxsParser_parseBitwiseXor(parser);

    if (HxsParser_maybe(parser, OR_BIT_TOKEN))
    {
        HxsExpr *right = HxsParser_parseBitwiseOr(parser);
        left = make_binop(left, right, BOP_OR);
    }

    return left;
}

HxsExpr *HxsParser_parseBitwiseXor(HxsParser *parser) // ^
{
    HxsExpr *left = HxsParser_parseBitwiseAnd(parser);

    if (HxsParser_maybe(parser, XOR_BIT_TOKEN))
    {
        HxsExpr *right = HxsParser_parseBitwiseXor(parser);
        left = make_binop(left, right, BOP_XOR);
    }

    return left;
}

HxsExpr *HxsParser_parseBitwiseAnd(HxsParser *parser) // &
{
    HxsExpr *left = HxsParser_parseEquality(parser);

    if (HxsParser_maybe(parser, AND_BIT_TOKEN))
    {
        HxsExpr *right = HxsParser_parseBitwiseAnd(parser);
        left = make_binop(left, right, BOP_AND);
    }

    return left;
}

HxsExpr *HxsParser_parseEquality(HxsParser *parser) // == !=
{
    HxsExpr *left = HxsParser_parseRelational(parser);

    if (HxsParser_maybe(parser, EQUAL_TOKEN))
    {
        HxsExpr *right = HxsParser_parseEquality(parser);
        left = make_binop(left, right, BOP_EQUAL);
    }
    else if (HxsParser_maybe(parser, NOT_EQUAL_TOKEN))
    {
        HxsExpr *right = HxsParser_parseEquality(parser);
        left = make_binop(left, right, BOP_NOT_EQUAL);
    }

    return left;
}
HxsExpr *HxsParser_parseRelational(HxsParser *parser) // < > <= >=
{
    HxsExpr *left = HxsParser_parseAdditive(parser);

    if (HxsParser_maybe(parser, GREATER_TOKEN))
    {
        HxsExpr *right = HxsParser_parseRelational(parser);
        left = make_binop(left, right, BOP_GREATER);
    }
    else if (HxsParser_maybe(parser, LESS_TOKEN))
    {
        HxsExpr *right = HxsParser_parseRelational(parser);
        left = make_binop(left, right, BOP_LESS);
    }
    else if (HxsParser_maybe(parser, GREATER_EQUAL_TOKEN))
    {
        HxsExpr *right = HxsParser_parseRelational(parser);
        left = make_binop(left, right, BOP_GREATER_EQUAL);
    }
    else if (HxsParser_maybe(parser, LESS_EQUAL_TOKEN))
    {
        HxsExpr *right = HxsParser_parseRelational(parser);
        left = make_binop(left, right, BOP_LESS_EQUAL);
    }

    return left;
}

HxsExpr *HxsParser_parseAdditive(HxsParser *parser) // + -
{
    HxsExpr *left = HxsParser_parseMultiplicative(parser);

    if (HxsParser_maybe(parser, ADD_TOKEN))
    {
        HxsExpr *right = HxsParser_parseAdditive(parser);
        left = make_binop(left, right, BOP_ADD);
    }
    else if (HxsParser_maybe(parser, MINUS_TOKEN))
    {
        HxsExpr *right = HxsParser_parseAdditive(parser);
        left = make_binop(left, right, BOP_SUB);
    }

    return left;
}

HxsExpr *HxsParser_parseMultiplicative(HxsParser *parser) // * / %
{
    HxsExpr *left = HxsParser_parseNullConditional(parser);

    if (HxsParser_maybe(parser, MULTIPLY_TOKEN))
    {
        HxsExpr *right = HxsParser_parseMultiplicative(parser);
        left = make_binop(left, right, BOP_MULT);
    }
    else if (HxsParser_maybe(parser, DIVIDE_TOKEN))
    {
        HxsExpr *right = HxsParser_parseMultiplicative(parser);
        left = make_binop(left, right, BOP_DIV);
    }
    else if (HxsParser_maybe(parser, MODULO_TOKEN))
    {
        HxsExpr *right = HxsParser_parseMultiplicative(parser);
        left = make_binop(left, right, BOP_MODULO);
    }

    return left;
}

HxsExpr *HxsParser_parseNullConditional(HxsParser *parser) // ?.
{
    HxsExpr *left = HxsParser_parsePrimitive(parser);

    if (HxsParser_maybe(parser, NULL_OPTIONAL_TOKEN))
    {
        HxsExpr *right = HxsParser_parseNullConditional(parser);
        left = make_binop(left, right, BOP_NULL);
    }

    return left;
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
    {
        HxsTokenKind found = token->kind;
        freeToken(token);
        HxsParser_throw(parser,
                        "Unexpected token '%s' where an expression was expected",
                        token_kind_name(found));
        return NULL;
    }
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

HxsType *HxsParser_parseType(HxsParser *parser)
{
    HxsToken *tok = HxsParser_expect(parser, IDENTIFIER_TOKEN);

    HxsType *type = malloc(sizeof(HxsType));
    type->kind = BASIC;
    size_t len = strlen(tok->value.str_val);
    type->basic.name = malloc(len + 1);
    memcpy(type->basic.name, tok->value.str_val, len + 1);
    type->basic.size = 0;
    type->basic.generics = NULL;
    freeToken(tok);

    if (HxsParser_maybe(parser, LESS_TOKEN))
    {
        type->basic.generics = malloc(0);

        while (!HxsParser_maybe(parser, GREATER_TOKEN))
        {
            int s = ++type->basic.size;
            HxsType **new = realloc(type->basic.generics, sizeof(HxsType *) * s);
            if (!new)
                HxsParser_throw(parser, "Failed generating memory.");

            type->basic.generics = new;
            type->basic.generics[s - 1] = HxsParser_parseType(parser);

            if (!HxsParser_maybe(parser, COMMA_TOKEN))
            {
                HxsParser_expect(parser, GREATER_TOKEN);
                break;
            }
        }
    }

    return type;
}

char *HxsParser_getIdent(HxsParser *parser)
{
    HxsToken *token = HxsParser_expect(parser, IDENTIFIER_TOKEN);

    size_t len = strlen(token->value.str_val);
    char *ident = malloc(len + 1);
    memcpy(ident, token->value.str_val, len + 1);

    freeToken(token);
    return ident;
}

void HxsParser_throw(HxsParser *parser, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(parser->error_msg, sizeof(parser->error_msg), fmt, args);
    va_end(args);

    parser->has_error = true;
    longjmp(parser->error_jmp, 1);
}

HxsToken *HxsParser_expect(HxsParser *parser, HxsTokenKind kind)
{
    HxsToken *tok = get_token(parser->lexer, false);

    if (tok->kind != kind)
    {
        HxsTokenKind found = tok->kind;
        freeToken(tok);

        HxsParser_throw(parser,
                        "Se esperaba %s pero se encontró %s",
                        token_kind_name(kind),
                        token_kind_name(found));

        return NULL;
    }

    freeToken(tok);
    return get_token(parser->lexer, true);
}

void freeParser(HxsParser *parser)
{
    free_lexer(parser->lexer);
    free(parser);
}