#include "core/hxs_utils.h"
#include "core/hxs_utils.h"
#include "ast/hxs_ast.h"
#include <stdio.h>

StringBuffer *Hxs_init_StringBuffer()
{
    StringBuffer *buf = malloc(sizeof(StringBuffer));
    buf->buf = malloc(0);
    buf->capacity = 0;
    buf->size = 0;

    return buf;
}
void Hxs_add_char_to_buffer(StringBuffer *buffer, char c)
{
    buffer->size++;
    if (buffer->size >= buffer->capacity)
    {
        char *new = realloc(buffer->buf, sizeof(char) * (buffer->size + 1));
        if (!new)
            return;
        buffer->buf = new;
        buffer->capacity = buffer->size + 1;
    }
    buffer->buf[buffer->size - 1] = c;
    buffer->buf[buffer->size] = '\0';
}
void Hxs_add_string_buffer(StringBuffer *buffer, const char *str)
{
    int len = strlen(str);

    for (size_t i = 0; i < len; i++)
    {
        Hxs_add_char_to_buffer(buffer, str[i]);
    }
}
char *Hxs_buffer_to_str(StringBuffer *buffer)
{
    return buffer->buf;
}

void Hxs_freeBuffer(StringBuffer *buffer)
{
    free(buffer->buf);
    free(buffer);
}

char *Hxs_strdup(HxsArena *arena, const char *str)
{
    if (!str)
    {
        return NULL;
    }

    size_t len = strlen(str) + 1;
    char *copy = Hxs_Arena_alloc(arena, len);

    if (copy)
    {
        memcpy(copy, str, len);
    }

    return copy;
}

static const char *kind_to_str(HxsExprKind kind)
{
    switch (kind)
    {
    case HXS_EXPR_CONST_INT:
        return "ConstInt";
    case HXS_EXPR_CONST_FLOAT:
        return "ConstFloat";
    case HXS_EXPR_CONST_STRING:
        return "ConstString";
    case HXS_EXPR_CONST_BOOL:
        return "ConstBool";
    case HXS_EXPR_CONST_NULL:
        return "ConstNull";
    case HXS_EXPR_IDENT:
        return "Identifier";
    case HXS_EXPR_FIELD:
        return "Field";
    case HXS_EXPR_INDEX_ACCESS:
        return "IndexAccess";
    case HXS_EXPR_BINOP:
        return "Binop";
    case HXS_EXPR_UNOP:
        return "Unop";
    case HXS_EXPR_ARRAY_DECL:
        return "ArrayDeclaration";
    case HXS_EXPR_OBJECT_DECL:
        return "ObjectDeclaration";
    case HXS_EXPR_CALL:
        return "Call";
    case HXS_EXPR_NEW:
        return "New";
    case HXS_EXPR_BLOCK:
        return "Block";
    case HXS_EXPR_IF:
        return "If";
    case HXS_EXPR_TERNARY:
        return "Ternary";
    case HXS_EXPR_SWITCH:
        return "Switch";
    case HXS_EXPR_FOR:
        return "For";
    case HXS_EXPR_WHILE:
        return "While";
    case HXS_EXPR_DO_WHILE:
        return "DoWhile";
    case HXS_EXPR_FUNCTION:
        return "Function";
    case HXS_EXPR_RETURN:
        return "Return";
    case HXS_EXPR_BREAK:
        return "Break";
    case HXS_EXPR_CONTINUE:
        return "Continue";
    case HXS_EXPR_THROW:
        return "Throw";
    case HXS_EXPR_TRY:
        return "Try";
    case HXS_EXPR_CAST:
        return "Cast";
    case HXS_EXPR_IS:
        return "Is";
    case HXS_EXPR_PARENTHESIS:
        return "Parenthesis";
    case HXS_EXPR_UNTYPED:
        return "Untyped";
    case HXS_EXPR_META:
        return "Meta";
    case HXS_EXPR_ASSIGN:
        return "Assign";
    case HXS_EXPR_ASSIGN_OP:
        return "AssignOp";
    case HXS_EXPR_VAR_DECL:
        return "VarDecl";
    default:
        return "Unknown";
    }
}

static const char *binop_to_str(HxsBinop op)
{
    switch (op)
    {
    case HXS_BINOP_ADD:
        return "+";
    case HXS_BINOP_SUB:
        return "-";
    case HXS_BINOP_MUL:
        return "*";
    case HXS_BINOP_DIV:
        return "/";
    case HXS_BINOP_MOD:
        return "%";
    case HXS_BINOP_ASSIGN_ADD:
        return "+=";
    case HXS_BINOP_ASSIGN_SUB:
        return "-=";
    case HXS_BINOP_ASSIGN_MUL:
        return "*=";
    case HXS_BINOP_ASSIGN_DIV:
        return "/=";
    case HXS_BINOP_ASSIGN_MOD:
        return "%=";
    case HXS_BINOP_EQ:
        return "==";
    case HXS_BINOP_NEQ:
        return "!=";
    case HXS_BINOP_LT:
        return "<";
    case HXS_BINOP_LE:
        return "<=";
    case HXS_BINOP_GT:
        return ">";
    case HXS_BINOP_GE:
        return ">=";
    case HXS_BINOP_AND:
        return "&&";
    case HXS_BINOP_OR:
        return "||";
    case HXS_BINOP_BIT_AND:
        return "&";
    case HXS_BINOP_BIT_OR:
        return "|";
    case HXS_BINOP_BIT_XOR:
        return "^";
    case HXS_BINOP_SHL:
        return "<<";
    case HXS_BINOP_SHR:
        return ">>";
    case HXS_BINOP_USHR:
        return ">>>";
    case HXS_BINOP_IN:
        return "in";
    case HXS_BINOP_SPREAD:
        return "...";
    default:
        return "unknown_op";
    }
}

static const char *unop_to_str(HxsUnop op)
{
    switch (op)
    {
    case HXS_UNOP_NEG:
        return "-";
    case HXS_UNOP_NOT:
        return "!";
    case HXS_UNOP_BIT_NOT:
        return "~";
    case HXS_UNOP_PRE_INC:
        return "++(pre)";
    case HXS_UNOP_PRE_DEC:
        return "--(pre)";
    case HXS_UNOP_POST_INC:
        return "(post)++";
    case HXS_UNOP_POST_DEC:
        return "(post)--";
    default:
        return "unknown_op";
    }
}

static void print_indent(int indent)
{
    for (int i = 0; i < indent; i++)
    {
        printf("  ");
    }
}

static void Hxs_printExprInternal(const HxsExpr *expr, int indent)
{
    if (!expr)
    {
        printf("null");
        return;
    }

    printf("{\n");
    int sub = indent + 1;

    print_indent(sub);
    printf("\"kind\": \"%s\",\n", kind_to_str(expr->kind));

    print_indent(sub);
    printf("\"pos\": {\"line\": %d, \"column\": %d},\n", expr->pos.line, expr->pos.column);

    print_indent(sub);
    printf("\"data\": ");

    switch (expr->kind)
    {
    case HXS_EXPR_CONST_INT:
        printf("{\n");
        print_indent(sub + 1);
        printf("\"value\": %lld\n", (long long)expr->data.ConstInt.value);
        print_indent(sub);
        printf("}\n");
        break;
    case HXS_EXPR_CONST_FLOAT:
        printf("{\n");
        print_indent(sub + 1);
        printf("\"value\": %f\n", expr->data.ConstFloat.value);
        print_indent(sub);
        printf("}\n");
        break;
    case HXS_EXPR_CONST_STRING:
        printf("{\n");
        print_indent(sub + 1);
        printf("\"value\": \"%s\"\n", expr->data.ConstString.value ? expr->data.ConstString.value : "null");
        print_indent(sub);
        printf("}\n");
        break;
    case HXS_EXPR_CONST_BOOL:
        printf("{\n");
        print_indent(sub + 1);
        printf("\"value\": %s\n", expr->data.ConstBool.value ? "true" : "false");
        print_indent(sub);
        printf("}\n");
        break;
    case HXS_EXPR_CONST_NULL:
        printf("{}\n");
        break;
    case HXS_EXPR_IDENT:
        printf("{\n");
        print_indent(sub + 1);
        printf("\"id\": \"%s\"\n", expr->data.Identifier.id ? expr->data.Identifier.id : "");
        print_indent(sub);
        printf("}\n");
        break;
    case HXS_EXPR_FIELD:
        printf("{\n");
        print_indent(sub + 1);
        printf("\"obj\": ");
        Hxs_printExprInternal(expr->data.Field.obj, sub + 1);
        printf(",\n");
        print_indent(sub + 1);
        printf("\"field\": \"%s\"\n", expr->data.Field.field ? expr->data.Field.field : "");
        print_indent(sub);
        printf("}\n");
        break;
    case HXS_EXPR_INDEX_ACCESS:
        printf("{\n");
        print_indent(sub + 1);
        printf("\"obj\": ");
        Hxs_printExprInternal(expr->data.Index.obj, sub + 1);
        printf(",\n");
        print_indent(sub + 1);
        printf("\"index\": ");
        Hxs_printExprInternal(expr->data.Index.index, sub + 1);
        printf("\n");
        print_indent(sub);
        printf("}\n");
        break;
    case HXS_EXPR_BINOP:
        printf("{\n");
        print_indent(sub + 1);
        printf("\"left\": ");
        Hxs_printExprInternal(expr->data.Binop.left, sub + 1);
        printf(",\n");
        print_indent(sub + 1);
        printf("\"op\": \"%s\",\n", binop_to_str(expr->data.Binop.op));
        print_indent(sub + 1);
        printf("\"right\": ");
        Hxs_printExprInternal(expr->data.Binop.right, sub + 1);
        printf("\n");
        print_indent(sub);
        printf("}\n");
        break;
    case HXS_EXPR_UNOP:
        printf("{\n");
        print_indent(sub + 1);
        printf("\"op\": \"%s\",\n", unop_to_str(expr->data.Unop.op));
        print_indent(sub + 1);
        printf("\"expr\": ");
        Hxs_printExprInternal(expr->data.Unop.expr, sub + 1);
        printf("\n");
        print_indent(sub);
        printf("}\n");
        break;
    case HXS_EXPR_ARRAY_DECL:
        printf("{\n");
        print_indent(sub + 1);
        printf("\"size\": %zu,\n", expr->data.ArrayDeclaration.size);
        print_indent(sub + 1);
        printf("\"body\": [\n");
        for (size_t i = 0; i < expr->data.ArrayDeclaration.size; i++)
        {
            print_indent(sub + 2);
            Hxs_printExprInternal(expr->data.ArrayDeclaration.body[i], sub + 2);
            if (i < expr->data.ArrayDeclaration.size - 1)
                printf(",\n");
            else
                printf("\n");
        }
        print_indent(sub + 1);
        printf("]\n");
        print_indent(sub);
        printf("}\n");
        break;
    case HXS_EXPR_OBJECT_DECL:
        printf("{\n");
        print_indent(sub + 1);
        printf("\"size\": %zu,\n", expr->data.ObjectDeclaration.size);
        print_indent(sub + 1);
        printf("\"values\": [\n");
        for (size_t i = 0; i < expr->data.ObjectDeclaration.size; i++)
        {
            print_indent(sub + 2);
            printf("{\n");
            print_indent(sub + 3);
            printf("\"name\": \"%s\",\n", expr->data.ObjectDeclaration.values[i]->name ? expr->data.ObjectDeclaration.values[i]->name : "");
            print_indent(sub + 3);
            printf("\"value\": ");
            Hxs_printExprInternal(expr->data.ObjectDeclaration.values[i]->value, sub + 3);
            printf("\n");
            print_indent(sub + 2);
            printf("}");
            if (i < expr->data.ObjectDeclaration.size - 1)
                printf(",\n");
            else
                printf("\n");
        }
        print_indent(sub + 1);
        printf("]\n");
        print_indent(sub);
        printf("}\n");
        break;
    case HXS_EXPR_CALL:
        printf("{\n");
        print_indent(sub + 1);
        printf("\"target\": ");
        Hxs_printExprInternal(expr->data.Call.target, sub + 1);
        printf(",\n");
        print_indent(sub + 1);
        printf("\"totalArgs\": %zu,\n", expr->data.Call.totalArgs);
        print_indent(sub + 1);
        printf("\"args\": [\n");
        for (size_t i = 0; i < expr->data.Call.totalArgs; i++)
        {
            print_indent(sub + 2);
            Hxs_printExprInternal(expr->data.Call.args[i], sub + 2);
            if (i < expr->data.Call.totalArgs - 1)
                printf(",\n");
            else
                printf("\n");
        }
        print_indent(sub + 1);
        printf("]\n");
        print_indent(sub);
        printf("}\n");
        break;
    case HXS_EXPR_NEW:
        printf("{\n");
        print_indent(sub + 1);
        printf("\"target\": \"%s\",\n", expr->data.New.target ? expr->data.New.target : "");
        print_indent(sub + 1);
        printf("\"totalArgs\": %zu,\n", expr->data.New.totalArgs);
        print_indent(sub + 1);
        printf("\"args\": [\n");
        for (size_t i = 0; i < expr->data.New.totalArgs; i++)
        {
            print_indent(sub + 2);
            Hxs_printExprInternal(expr->data.New.args[i], sub + 2);
            if (i < expr->data.New.totalArgs - 1)
                printf(",\n");
            else
                printf("\n");
        }
        print_indent(sub + 1);
        printf("]\n");
        print_indent(sub);
        printf("}\n");
        break;
    case HXS_EXPR_BLOCK:
        printf("{\n");
        print_indent(sub + 1);
        printf("\"size\": %zu,\n", expr->data.Block.size);
        print_indent(sub + 1);
        printf("\"body\": [\n");
        for (size_t i = 0; i < expr->data.Block.size; i++)
        {
            print_indent(sub + 2);
            Hxs_printExprInternal(expr->data.Block.body[i], sub + 2);
            if (i < expr->data.Block.size - 1)
                printf(",\n");
            else
                printf("\n");
        }
        print_indent(sub + 1);
        printf("]\n");
        print_indent(sub);
        printf("}\n");
        break;
    case HXS_EXPR_IF:
        printf("{\n");
        print_indent(sub + 1);
        printf("\"cond\": ");
        Hxs_printExprInternal(expr->data.If.cond, sub + 1);
        printf(",\n");
        print_indent(sub + 1);
        printf("\"value\": ");
        Hxs_printExprInternal(expr->data.If.value, sub + 1);
        printf(",\n");
        print_indent(sub + 1);
        printf("\"elsee\": ");
        Hxs_printExprInternal(expr->data.If.elsee, sub + 1);
        printf("\n");
        print_indent(sub);
        printf("}\n");
        break;
    case HXS_EXPR_TERNARY:
        printf("{\n");
        print_indent(sub + 1);
        printf("\"cond\": ");
        Hxs_printExprInternal(expr->data.Ternary.cond, sub + 1);
        printf(",\n");
        print_indent(sub + 1);
        printf("\"onTrue\": ");
        Hxs_printExprInternal(expr->data.Ternary.onTrue, sub + 1);
        printf(",\n");
        print_indent(sub + 1);
        printf("\"onFalse\": ");
        Hxs_printExprInternal(expr->data.Ternary.onFalse, sub + 1);
        printf("\n");
        print_indent(sub);
        printf("}\n");
        break;
    case HXS_EXPR_SWITCH:
        printf("{\n");
        print_indent(sub + 1);
        printf("\"target\": ");
        Hxs_printExprInternal(expr->data.Switch.target, sub + 1);
        printf("\n");
        print_indent(sub);
        printf("}\n");
        break;
    case HXS_EXPR_FOR:
        printf("{\n");
        print_indent(sub + 1);
        printf("\"variable\": \"%s\",\n", expr->data.For.variable ? expr->data.For.variable : "");
        print_indent(sub + 1);
        printf("\"target\": ");
        Hxs_printExprInternal(expr->data.For.target, sub + 1);
        printf(",\n");
        print_indent(sub + 1);
        printf("\"body\": ");
        Hxs_printExprInternal(expr->data.For.body, sub + 1);
        printf("\n");
        print_indent(sub);
        printf("}\n");
        break;
    case HXS_EXPR_WHILE:
        printf("{\n");
        print_indent(sub + 1);
        printf("\"cond\": ");
        Hxs_printExprInternal(expr->data.While.cond, sub + 1);
        printf(",\n");
        print_indent(sub + 1);
        printf("\"body\": ");
        Hxs_printExprInternal(expr->data.While.body, sub + 1);
        printf("\n");
        print_indent(sub);
        printf("}\n");
        break;
    case HXS_EXPR_DO_WHILE:
        printf("{\n");
        print_indent(sub + 1);
        printf("\"cond\": ");
        Hxs_printExprInternal(expr->data.DoWhile.cond, sub + 1);
        printf(",\n");
        print_indent(sub + 1);
        printf("\"body\": ");
        Hxs_printExprInternal(expr->data.DoWhile.body, sub + 1);
        printf("\n");
        print_indent(sub);
        printf("}\n");
        break;
    default:
        printf("{}\n");
        break;
    }

    print_indent(indent);
    printf("}");
}

void Hxs_printExpr(HxsExpr *expr)
{
    Hxs_printExprInternal(expr, 0);
    printf("\n");
}