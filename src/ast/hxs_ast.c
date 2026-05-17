#include "ast/hxs_ast.h"

HxsExpr *Hxs_Expr_makeExpression(HxsExprKind kind, HxsPosition pos)
{
    HxsExpr *expr = malloc(sizeof(HxsExpr));
    expr->kind = kind;
    expr->pos = pos;

    return expr;
}

HxsExpr *Hxs_Expr_makeIntLiteral(HxsPosition pos, int64_t value)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(HXS_EXPR_CONST_INT, pos);
    expr->data.ConstInt.value = value;

    return expr;
}

HxsExpr *Hxs_Expr_makeFloatLiteral(HxsPosition pos, double value)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(HXS_EXPR_CONST_FLOAT, pos);
    expr->data.ConstFloat.value = value;

    return expr;
}

HxsExpr *Hxs_Expr_makeStringLiteral(HxsPosition pos, char *value)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(HXS_EXPR_CONST_STRING, pos);
    size_t len = strlen(value) + 1;
    expr->data.ConstString.value = malloc(sizeof(char) * len);
    memcpy(expr->data.ConstString.value, value, sizeof(char) * len);

    return expr;
}

HxsExpr *Hxs_Expr_makeBooleanLiteral(HxsPosition pos, bool value)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(HXS_EXPR_CONST_BOOL, pos);
    expr->data.ConstBool.value = value;

    return expr;
}

HxsExpr *Hxs_Expr_makeIdentifier(HxsPosition pos, char *id)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(HXS_EXPR_IDENT, pos);
    size_t len = strlen(id) + 1;
    expr->data.Identifier.id = malloc(sizeof(char) * len);
    memcpy(expr->data.Identifier.id, id, sizeof(char) * len);

    return expr;
}

HxsExpr *Hxs_Expr_makeField(HxsPosition pos, HxsExpr *parent, char *field)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(HXS_EXPR_FIELD, pos);
    expr->data.Field.obj = parent;
    size_t len = strlen(field) + 1;
    expr->data.Field.field = malloc(sizeof(char) * len);
    memcpy(expr->data.Field.field, field, sizeof(char) * len);

    return expr;
}

HxsExpr *Hxs_Expr_makeIndex(HxsPosition pos, HxsExpr *parent, HxsExpr *index)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(HXS_EXPR_INDEX_ACCESS, pos);
    expr->data.Index.obj = parent;
    expr->data.Index.index = index;

    return expr;
}

HxsExpr *Hxs_Expr_makeBinop(HxsPosition pos, HxsBinop binop, HxsExpr *left, HxsExpr *right)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(HXS_EXPR_BINOP, pos);
    expr->data.Binop.op = binop;
    expr->data.Binop.left = left;
    expr->data.Binop.right = right;

    return expr;
}

HxsExpr *Hxs_Expr_makeUnop(HxsPosition pos, HxsUnop unop, HxsExpr *value)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(HXS_EXPR_UNOP, pos);
    expr->data.Unop.op = unop;
    expr->data.Unop.expr = value;

    return expr;
}

HxsExpr *Hxs_Expr_makeArrayDeclaration(HxsPosition pos, size_t size, HxsExpr **values)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(HXS_EXPR_ARRAY_DECL, pos);
    expr->data.ArrayDeclaration.size = size;
    expr->data.ArrayDeclaration.body = values;

    return expr;
}

HxsExpr *Hxs_Expr_makeObjDeclaration(HxsPosition pos, size_t size, HxsObjField **values)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(HXS_EXPR_OBJECT_DECL, pos);
    expr->data.ObjectDeclaration.size = size;
    expr->data.ObjectDeclaration.values = values;

    return expr;
}

HxsExpr *Hxs_Expr_makeCall(HxsPosition pos, HxsExpr *target, size_t size, HxsExpr **values)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(HXS_EXPR_CALL, pos);
    expr->data.Call.target = target;
    expr->data.Call.totalArgs = size;
    expr->data.Call.args = values;

    return expr;
}

HxsExpr *Hxs_Expr_makeNew(HxsPosition pos, char *target, size_t size, HxsExpr **values)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(HXS_EXPR_NEW, pos);
    expr->data.New.target = target;
    expr->data.New.totalArgs = size;
    expr->data.New.args = values;

    return expr;
}

HxsExpr *Hxs_Expr_makeBlock(HxsPosition pos, size_t size, HxsExpr **body)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(HXS_EXPR_BLOCK, pos);
    expr->data.Block.size = size;
    expr->data.Block.body = body;

    return expr;
}

HxsExpr *Hxs_Expr_makeIf(HxsPosition pos, HxsExpr *cond, HxsExpr *body, HxsExpr *elsee)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(HXS_EXPR_IF, pos);
    expr->data.If.cond = cond;
    expr->data.If.value = body;
    expr->data.If.elsee = elsee;

    return expr;
}

HxsExpr *Hxs_Expr_makeTernary(HxsPosition pos, HxsExpr *cond, HxsExpr *onTrue, HxsExpr *onFalse)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(HXS_EXPR_TERNARY, pos);
    expr->data.Ternary.cond = cond;
    expr->data.Ternary.onTrue = onTrue;
    expr->data.Ternary.onFalse = onFalse;

    return expr;
}

HxsExpr *Hxs_Expr_makeSwitch(HxsPosition pos, HxsExpr *target)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(HXS_EXPR_SWITCH, pos);
    expr->data.Switch.target = target;

    return expr;
}

HxsExpr *Hxs_Expr_makeFor(HxsPosition pos, char *variable, HxsExpr *target, HxsExpr *body)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(HXS_EXPR_FOR, pos);
    size_t len = strlen(variable) + 1;
    expr->data.For.variable = malloc(sizeof(char) * len);
    memcpy(expr->data.For.variable, variable, sizeof(char) * len);
    expr->data.For.target = target;
    expr->data.For.body = body;

    return expr;
}

HxsExpr *Hxs_Expr_makeWhile(HxsPosition pos, HxsExpr *cond, HxsExpr *body)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(HXS_EXPR_WHILE, pos);
    expr->data.While.cond = cond;
    expr->data.While.body = body;

    return expr;
}

HxsExpr *Hxs_Expr_makeDoWhile(HxsPosition pos, HxsExpr *cond, HxsExpr *body)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(HXS_EXPR_DO_WHILE, pos);
    expr->data.DoWhile.cond = cond;
    expr->data.DoWhile.body = body;

    return expr;
}

void Hxs_free_Expr(HxsExpr *expr)
{
    if (!expr)
        return;

    switch (expr->kind)
    {
    case HXS_EXPR_CONST_STRING:
        free(expr->data.ConstString.value);
        break;

    case HXS_EXPR_IDENT:
        free(expr->data.Identifier.id);
        break;

    case HXS_EXPR_FIELD:
        Hxs_free_Expr(expr->data.Field.obj);
        free(expr->data.Field.field);
        break;

    case HXS_EXPR_INDEX_ACCESS:
        Hxs_free_Expr(expr->data.Index.obj);
        Hxs_free_Expr(expr->data.Index.index);
        break;

    case HXS_EXPR_BINOP:
        Hxs_free_Expr(expr->data.Binop.left);
        Hxs_free_Expr(expr->data.Binop.right);
        break;

    case HXS_EXPR_UNOP:
        Hxs_free_Expr(expr->data.Unop.expr);
        break;

    case HXS_EXPR_ARRAY_DECL:
        for (size_t i = 0; i < expr->data.ArrayDeclaration.size; i++)
            Hxs_free_Expr(expr->data.ArrayDeclaration.body[i]);
        free(expr->data.ArrayDeclaration.body);
        break;

    case HXS_EXPR_OBJECT_DECL:
        for (size_t i = 0; i < expr->data.ObjectDeclaration.size; i++)
            Hxs_freeObjField(expr->data.ObjectDeclaration.values[i]);
        free(expr->data.ObjectDeclaration.values);
        break;

    default:
        break;
    }

    free(expr);
}