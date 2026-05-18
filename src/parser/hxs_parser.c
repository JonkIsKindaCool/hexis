#include "parser/hxs_parser.h"

#include <stdarg.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>

static Hxs_BinopPrecedence BINOP_PRECEDENCE[] = {
    // Multiplicative
    {HXS_BINOP_MUL, 12},
    {HXS_BINOP_DIV, 12},
    {HXS_BINOP_MOD, 12},

    // Additive
    {HXS_BINOP_ADD, 11},
    {HXS_BINOP_SUB, 11},

    // Shift
    {HXS_BINOP_SHL, 10},
    {HXS_BINOP_SHR, 10},
    {HXS_BINOP_USHR, 10},

    // Relational
    {HXS_BINOP_LT, 9},
    {HXS_BINOP_LE, 9},
    {HXS_BINOP_GT, 9},
    {HXS_BINOP_GE, 9},

    // Equality
    {HXS_BINOP_EQ, 8},
    {HXS_BINOP_NEQ, 8},

    // Bitwise
    {HXS_BINOP_BIT_AND, 7},
    {HXS_BINOP_BIT_XOR, 6},
    {HXS_BINOP_BIT_OR, 5},

    // Logical
    {HXS_BINOP_AND, 4},
    {HXS_BINOP_OR, 3},

    // Assignment (right-associative)
    {HXS_BINOP_ASSIGN_ADD, 2},
    {HXS_BINOP_ASSIGN_SUB, 2},
    {HXS_BINOP_ASSIGN_MUL, 2},
    {HXS_BINOP_ASSIGN_DIV, 2},
    {HXS_BINOP_ASSIGN_MOD, 2},

    // Lowest
    {HXS_BINOP_IN, 1},
    {HXS_BINOP_SPREAD, 1}};

static void parser_error(HxsParser *p, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(p->error_msg, sizeof(p->error_msg), fmt, args);
    va_end(args);

    const char *file = p->filename ? p->filename : "<unknown>";
    int line = 1, col = 1;
    if (p->current)
    {
        line = (int)p->current->line;
        col = (int)p->current->start;
    }
    else if (p->peek)
    {
        line = (int)p->peek->line;
        col = (int)p->peek->start;
    }
    fprintf(stderr, "%s:%d:%d: error: %s\n", file, line, col, p->error_msg);
    p->has_error = true;
    longjmp(p->error_jmp, 1);
}

static void advance(HxsParser *p)
{
    p->current = p->peek;
    p->peek = Hxs_get_token(p->lexer, true);
}

static void expect(HxsParser *p, HxsTokenKind kind)
{
    if (p->current->kind != kind)
        parser_error(p, "expected %s, got %s", token_kind_name(kind), token_kind_name(p->current->kind));
}

static void expect_op(HxsParser *p, HxsOpKind kind)
{
    if (p->current->kind != OP_TOKEN || p->current->value.op != kind)
        parser_error(p, "expected %s, got %s", token_kind_name(kind), token_kind_name(p->current->kind));
}

static bool match(HxsParser *p, HxsTokenKind kind)
{
    if (p->current->kind == kind)
    {
        advance(p);
        return true;
    }
    return false;
}

static int get_precedence(HxsBinop op)
{
    size_t num_ops = sizeof(BINOP_PRECEDENCE) / sizeof(BINOP_PRECEDENCE[0]);
    for (size_t i = 0; i < num_ops; i++)
    {
        if (BINOP_PRECEDENCE[i].op == op)
            return BINOP_PRECEDENCE[i].precedence;
    }
    return 0;
}

static HxsBinop token_to_binop(HxsToken *tok)
{
    if (tok->kind != OP_TOKEN)
        return -1;
    switch (tok->value.op)
    {
    case HXS_OP_ADD:
        return HXS_BINOP_ADD;
    case HXS_OP_SUB:
        return HXS_BINOP_SUB;
    case HXS_OP_MUL:
        return HXS_BINOP_MUL;
    case HXS_OP_DIV:
        return HXS_BINOP_DIV;
    case HXS_OP_MOD:
        return HXS_BINOP_MOD;
    case HXS_OP_ADD_ASSIGN:
        return HXS_BINOP_ASSIGN_ADD;
    case HXS_OP_SUB_ASSIGN:
        return HXS_BINOP_ASSIGN_SUB;
    case HXS_OP_MUL_ASSIGN:
        return HXS_BINOP_ASSIGN_MUL;
    case HXS_OP_DIV_ASSIGN:
        return HXS_BINOP_ASSIGN_DIV;
    case HXS_OP_MOD_ASSIGN:
        return HXS_BINOP_ASSIGN_MOD;
    case HXS_OP_EQ:
        return HXS_BINOP_EQ;
    case HXS_OP_NEQ:
        return HXS_BINOP_NEQ;
    case HXS_OP_LT:
        return HXS_BINOP_LT;
    case HXS_OP_LE:
        return HXS_BINOP_LE;
    case HXS_OP_GT:
        return HXS_BINOP_GT;
    case HXS_OP_GE:
        return HXS_BINOP_GE;
    case HXS_OP_AND:
        return HXS_BINOP_AND;
    case HXS_OP_OR:
        return HXS_BINOP_OR;
    case HXS_OP_BIT_AND:
        return HXS_BINOP_BIT_AND;
    case HXS_OP_BIT_OR:
        return HXS_BINOP_BIT_OR;
    case HXS_OP_BIT_XOR:
        return HXS_BINOP_BIT_XOR;
    default:
        return -1;
    }
}

static HxsUnop token_to_unop(HxsToken *tok)
{
    if (tok->kind != OP_TOKEN)
        return -1;
    switch (tok->value.op)
    {
    case HXS_OP_SUB:
        return HXS_UNOP_NEG;
    case HXS_OP_NOT:
        return HXS_UNOP_NOT;
    case HXS_OP_BIT_NOT:
        return HXS_UNOP_BIT_NOT;
    default:
        return -1;
    }
}

static HxsExpr *parse_top_level(HxsParser *p);
static HxsExpr *parse_expression(HxsParser *p, int min_precedence);
static HxsExpr *parse_primary(HxsParser *p);

static HxsAstType *parse_type(HxsParser *p);

static HxsExpr *parse_binary(HxsParser *p, HxsExpr *left, int min_precedence)
{
    HxsBinop op = token_to_binop(p->current);
    int prec = get_precedence(op);
    advance(p);

    int next_min_precedence = (prec == 2) ? prec : prec + 1;
    HxsExpr *right = parse_expression(p, next_min_precedence);

    return Hxs_Expr_makeBinop(p->arena, left->pos, op, left, right);
}

static HxsExpr *parse_unary(HxsParser *p)
{
    HxsUnop op = token_to_unop(p->current);
    HxsPosition pos = {.line = p->current->line, .column = p->current->start};
    advance(p);
    HxsExpr *operand = parse_expression(p, 14);
    return Hxs_Expr_makeUnop(p->arena, pos, op, operand);
}

static HxsExpr *parse_top_level(HxsParser *p)
{
    HxsPosition pos = {.line = p->current->line, .column = p->current->start};
    HxsExpr *expr = NULL;

    switch (p->current->kind)
    {
    case CONST_TOKEN:
    case VAR_TOKEN:
    {
        bool isConst = p->current->kind == CONST_TOKEN;

        advance(p);

        expect(p, IDENTIFIER_TOKEN);

        char *name = p->current->value.str_val;

        advance(p);

        HxsAstType *type = NULL;
        HxsExpr *value = NULL;

        if (p->current->kind == COLON_TOKEN)
        {
            advance(p);
            type = parse_type(p);
        }

        if (isConst)
        {
            expect_op(p, HXS_OP_ASSIGN);
            advance(p);

            value = parse_expression(p, 0);
        }
        else
        {
            if (p->current->kind == OP_TOKEN &&
                p->current->value.op == HXS_OP_ASSIGN)
            {
                advance(p);

                value = parse_expression(p, 0);
            }
            else
            {
                if (type == NULL)
                {
                    parser_error(p,
                                 "expected initializer or type annotation");
                }
            }
        }

        return Hxs_Expr_makeVar(
            p->arena,
            pos,
            isConst,
            name,
            type,
            value);
    }
    default:
        expr = parse_expression(p, 0);
        break;
    }

    return expr;
}

static HxsExpr *parse_primary(HxsParser *p)
{
    HxsPosition pos = {.line = p->current->line, .column = p->current->start};
    HxsExpr *expr = NULL;

    switch (p->current->kind)
    {
    case INT_TOKEN:
        expr = Hxs_Expr_makeIntLiteral(p->arena, pos, p->current->value.int_val);
        advance(p);
        break;
    case FLOAT_TOKEN:
        expr = Hxs_Expr_makeFloatLiteral(p->arena, pos, p->current->value.float_val);
        advance(p);
        break;
    case DOUBLE_QUOTE_STRING_TOKEN:
    case SINGLE_QUOTE_STRING_TOKEN:
        expr = Hxs_Expr_makeStringLiteral(p->arena, pos, p->current->value.str_val);
        advance(p);
        break;
    case TRUE_TOKEN:
        expr = Hxs_Expr_makeBooleanLiteral(p->arena, pos, true);
        advance(p);
        break;
    case FALSE_TOKEN:
        expr = Hxs_Expr_makeBooleanLiteral(p->arena, pos, false);
        advance(p);
        break;
    case IDENTIFIER_TOKEN:
        expr = Hxs_Expr_makeIdentifier(p->arena, pos, p->current->value.str_val);
        advance(p);
        break;
    case LEFTPAREN_TOKEN:
        advance(p);
        expr = parse_expression(p, 0);
        expect(p, RIGHTPAREN_TOKEN);
        advance(p);
        break;
    case LEFTBRACKET_TOKEN:
    { // Array literal
        advance(p);
        size_t cap = 8, count = 0;
        HxsExpr **elems = Hxs_Arena_alloc(p->arena, cap * sizeof(HxsExpr *));
        while (p->current->kind != RIGHTBRACKET_TOKEN && p->current->kind != EOF_TOKEN)
        {
            if (count >= cap)
            {
                size_t old_cap = cap;
                cap *= 2;
                HxsExpr **new_elems = Hxs_Arena_alloc(p->arena, cap * sizeof(HxsExpr *));
                memcpy(new_elems, elems, old_cap * sizeof(HxsExpr *));
                elems = new_elems;
            }
            elems[count++] = parse_expression(p, 0);
            if (!match(p, COMMA_TOKEN))
                break;
        }
        expect(p, RIGHTBRACKET_TOKEN);
        advance(p);
        expr = Hxs_Expr_makeArrayDeclaration(p->arena, pos, count, elems);
        break;
    }
    case LEFTCURLYBRACKET_TOKEN:
    { // Object literal
        advance(p);
        size_t cap = 8, count = 0;
        HxsObjField **fields = Hxs_Arena_alloc(p->arena, cap * sizeof(HxsObjField *));
        while (p->current->kind != RIGHTCURLYBRACKET_TOKEN && p->current->kind != EOF_TOKEN)
        {
            if (count >= cap)
            {
                size_t old_cap = cap;
                cap *= 2;
                HxsObjField **new_fields = Hxs_Arena_alloc(p->arena, cap * sizeof(HxsObjField *));
                memcpy(new_fields, fields, old_cap * sizeof(HxsObjField *));
                fields = new_fields;
            }
            expect(p, IDENTIFIER_TOKEN);
            char *name = p->current->value.str_val;
            advance(p);
            expect(p, COLON_TOKEN);
            advance(p);
            HxsExpr *value = parse_expression(p, 0);
            fields[count++] = Hxs_makeObjField(p->arena, name, value);
            if (!match(p, COMMA_TOKEN))
                break;
        }
        expect(p, RIGHTCURLYBRACKET_TOKEN);
        advance(p);
        expr = Hxs_Expr_makeObjDeclaration(p->arena, pos, count, fields);
        break;
    }
    default:
        parser_error(p, "unexpected token in primary: %s", token_kind_name(p->current->kind));
        return NULL;
    }
    return expr;
}

static HxsExpr *parse_expression(HxsParser *p, int min_precedence)
{
    HxsExpr *left = NULL;

    if (p->current->kind == OP_TOKEN && token_to_unop(p->current) != -1)
    {
        left = parse_unary(p);
    }
    else
    {
        left = parse_primary(p);
    }

    while (p->current->kind == OP_TOKEN && token_to_binop(p->current) != -1)
    {
        HxsBinop op = token_to_binop(p->current);
        int prec = get_precedence(op);
        if (prec < min_precedence)
            break;
        left = parse_binary(p, left, prec);
    }
    return left;
}

static HxsAstType *parse_type(HxsParser *p)
{
    HxsAstType *type = Hxs_Type_makeType(p->arena, HXS_BASIC_TYPE);

    expect(p, IDENTIFIER_TOKEN);
    advance(p);

    return type;
}

static bool expr_requires_semicolon(HxsExpr *expr)
{
    if (!expr)
        return true;

    return true;
}

HxsExpr *Hxs_Parser_parse_program(HxsParser *p)
{
    if (setjmp(p->error_jmp) != 0)
        return NULL;

    size_t cap = 32, count = 0;
    HxsExpr **body = Hxs_Arena_alloc(p->arena, cap * sizeof(HxsExpr *));
    while (p->current->kind != EOF_TOKEN)
    {
        if (count >= cap)
        {
            size_t old_cap = cap;
            cap *= 2;
            HxsExpr **new_body = Hxs_Arena_alloc(p->arena, cap * sizeof(HxsExpr *));
            memcpy(new_body, body, old_cap * sizeof(HxsExpr *));
            body = new_body;
        }

        HxsExpr *expr = parse_top_level(p);
        body[count++] = expr;

        if (match(p, SEMICOLON_TOKEN))
            continue;

        if (!expr_requires_semicolon(expr) || p->current->kind == EOF_TOKEN)
            continue;

        parser_error(p, "expected ';' after expression, got %s at line %d col %d",
                     token_kind_name(p->current->kind),
                     (int)p->current->line, (int)p->current->start);
    }
    HxsPosition dummy = {0};
    return Hxs_Expr_makeBlock(p->arena, dummy, count, body);
}

HxsParser *Hxs_Parser_new(HxsArena *arena, HxsLexer *lexer, const char *filename)
{
    HxsParser *p = Hxs_Arena_alloc(arena, sizeof(HxsParser));
    p->arena = arena;
    p->lexer = lexer;
    p->filename = filename ? filename : "<unknown>";

    p->current = Hxs_get_token(lexer, true);
    p->peek = Hxs_get_token(lexer, true);

    p->has_error = false;
    return p;
}