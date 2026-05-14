#include "hxs_ast.h"

HxsExpr *make_base_expr(HXS_EXPR_KIND kind)
{
    HxsExpr *expr = (HxsExpr *)malloc(sizeof(HxsExpr));
    expr->kind = kind;

    return expr;
}

HxsExpr *make_int_literal(int64_t value)
{
    HxsExpr *expr = make_base_expr(INT_EXPR);
    expr->int_p.value = value;

    return expr;
}

HxsExpr *make_bool_literal(bool value)
{
    HxsExpr *expr = make_base_expr(BOOLEAN_EXPR);
    expr->bool_p.value = value;

    return expr;
}

HxsExpr *make_float_literal(double value)
{
    HxsExpr *expr = make_base_expr(FLOAT_EXPR);
    expr->float_p.value = value;

    return expr;
}
HxsExpr *make_string_literal(char *value, bool single)
{
    HxsExpr *expr = make_base_expr(single ? SINGLE_STRING_EXPR : DOUBLE_STRING_EXPR);
    expr->string_p.value = value;

    return expr;
}
HxsExpr *make_identifier_literal(char *value)
{
    HxsExpr *expr = make_base_expr(IDENTIFIER_EXPR);
    expr->string_p.value = value;

    return expr;
}
HxsExpr *make_binop(HxsExpr *left, HxsExpr *right, Binary_Kind op)
{
    HxsExpr *expr = make_base_expr(BINARY_OP);
    expr->binary_op.left = left;
    expr->binary_op.right = right;
    expr->binary_op.op = op;

    return expr;
}

void free_expr(HxsExpr *expr)
{
    switch (expr->kind)
    {
    case SINGLE_STRING_EXPR:
    case DOUBLE_STRING_EXPR:
    case IDENTIFIER_EXPR:
        free(expr->string_p.value);
        break;
    case BINARY_OP:
        free_expr(expr->binary_op.left);
        free_expr(expr->binary_op.right);
        break;
    default:
        break;
    }
    free(expr);
}

// STATMENETS

HxsStmt *make_base_stmt(HXS_STMT_KIND kind)
{
    HxsStmt *stmt = malloc(sizeof(HxsStmt));
    stmt->kind = kind;

    return stmt;
}

HxsStmt *make_expr_stmt(HxsExpr *value)
{
    HxsStmt *stmt = make_base_stmt(EXPR_STMT);
    stmt->expr.expression = value;

    return stmt;
}

void free_stmt(HxsStmt *stmt)
{
    switch (stmt->kind)
    {
    case EXPR_STMT:
        free_expr(stmt->expr.expression);
        break;
    case BLOCK_STMT:
        for (size_t i = 0; i < stmt->body.size; i++)
        {
            free_stmt(stmt->body.body[i]);
        }
        free(stmt->body.body);
        break;                 

    default:
        break;
    }
    free(stmt);
}