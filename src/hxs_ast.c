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
    size_t len = strlen(value);
    expr->string_p.value = malloc(len + 1);     
    memcpy(expr->string_p.value, value, len + 1);

    return expr;
}

HxsExpr *make_identifier_literal(char *value)
{
    HxsExpr *expr = make_base_expr(IDENTIFIER_EXPR);
    size_t len = strlen(value);
    expr->string_p.value = malloc(len + 1);
    memcpy(expr->string_p.value, value, len + 1);

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

StringBuffer *print_expr(HxsExpr *expr, int spaces)
{
    StringBuffer *buf = init_StringBuffer();
#define ADD_SPACES(sp)                \
    for (size_t i = 0; i < sp; i++)   \
    {                                 \
        add_char_to_buffer(buf, ' '); \
    }

    ADD_SPACES(spaces);
    add_string_buffer(buf, "{\n");
    switch (expr->kind)
    {
    case INT_EXPR:
    {
        ADD_SPACES(spaces + 2);
        add_string_buffer(buf, "Kind: Int Expression\n");
        char *value = malloc(256);
        snprintf(value, 256, "Value: %ld\n", expr->int_p.value);

        ADD_SPACES(spaces + 2);
        add_string_buffer(buf, value);

        free(value);
        break;
    }
    case FLOAT_EXPR:
    {
        ADD_SPACES(spaces + 2);
        add_string_buffer(buf, "Kind: Float Expression\n");
        char *value = malloc(256);
        snprintf(value, 256, "Value: %g\n", expr->float_p.value);

        ADD_SPACES(spaces + 2);
        add_string_buffer(buf, value);

        free(value);
        break;
    }
    case IDENTIFIER_EXPR:
    {
        ADD_SPACES(spaces + 2);
        add_string_buffer(buf, "Kind: Identifier Expression\n");
        char *value = malloc(256);
        snprintf(value, 256, "Value: %s\n", expr->string_p.value);

        ADD_SPACES(spaces + 2);
        add_string_buffer(buf, value);

        free(value);
        break;
    }
    case SINGLE_STRING_EXPR:
    case DOUBLE_STRING_EXPR:
    {
        ADD_SPACES(spaces + 2);
        add_string_buffer(buf, "Kind: String Expression\n");
        char *value = malloc(256);
        snprintf(value, 256, "Value: %s\n", expr->string_p.value);

        ADD_SPACES(spaces + 2);
        add_string_buffer(buf, value);

        free(value);
        break;
    }
    default:
        break;
    }

    ADD_SPACES(spaces);
    add_char_to_buffer(buf, '}');
    add_char_to_buffer(buf, '\n');

    return buf;
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

StringBuffer *print_stmt(HxsStmt *stmt, int spaces)
{
    StringBuffer *buf = init_StringBuffer();
#define ADD_SPACES(sp)                \
    for (size_t i = 0; i < sp; i++)   \
    {                                 \
        add_char_to_buffer(buf, ' '); \
    }

    ADD_SPACES(spaces);
    add_string_buffer(buf, "{\n");
    switch (stmt->kind)
    {
    case BLOCK_STMT:
        ADD_SPACES(spaces + 2);
        add_string_buffer(buf, "Kind: StatementBlock\n");
        char *size = malloc(256);
        snprintf(size, 256, "Size: %i\n", stmt->body.size);

        ADD_SPACES(spaces + 2);
        add_string_buffer(buf, size);

        ADD_SPACES(spaces + 2);
        add_string_buffer(buf, "Body:\n");

        for (size_t i = 0; i < stmt->body.size; i++)
        {
            StringBuffer *temp = print_stmt(stmt->body.body[i], spaces + 4);

            add_string_buffer(buf, temp->buf);

            freeBuffer(temp);
        }

        free(size);
        break;
    case EXPR_STMT:
        ADD_SPACES(spaces + 2);
        add_string_buffer(buf, "Kind: Expression Statement\n");
        ADD_SPACES(spaces + 2);
        add_string_buffer(buf, "Expression:\n");
        StringBuffer *temp = print_expr(stmt->expr.expression, spaces + 4);

        add_string_buffer(buf, temp->buf);

        freeBuffer(temp);
        break;

    default:
        break;
    }

    ADD_SPACES(spaces);
    add_char_to_buffer(buf, '}');
    add_char_to_buffer(buf, '\n');

    return buf;
}