#include "ast/hxs_ast.h"

HxsExpr *Hxs_Expr_makeExpression(HxsArena* arena, HxsExprKind kind, HxsPosition pos)
{
    HxsExpr *expr = Hxs_Arena_alloc(arena, sizeof(HxsExpr));
    expr->kind = kind;
    expr->pos = pos;

    return expr;
}

HxsExpr *Hxs_Expr_makeIntLiteral(HxsArena* arena, HxsPosition pos, int64_t value)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(arena, HXS_EXPR_CONST_INT, pos);
    expr->data.ConstInt.value = value;

    return expr;
}

HxsExpr *Hxs_Expr_makeFloatLiteral(HxsArena* arena, HxsPosition pos, double value)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(arena, HXS_EXPR_CONST_FLOAT, pos);
    expr->data.ConstFloat.value = value;

    return expr;
}

HxsExpr *Hxs_Expr_makeStringLiteral(HxsArena* arena, HxsPosition pos, char *value)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(arena, HXS_EXPR_CONST_STRING, pos);
    size_t len = strlen(value) + 1;
    expr->data.ConstString.value = Hxs_strdup(arena, value);

    return expr;
}

HxsExpr *Hxs_Expr_makeBooleanLiteral(HxsArena* arena, HxsPosition pos, bool value)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(arena, HXS_EXPR_CONST_BOOL, pos);
    expr->data.ConstBool.value = value;

    return expr;
}

HxsExpr *Hxs_Expr_makeIdentifier(HxsArena* arena, HxsPosition pos, char *id)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(arena, HXS_EXPR_IDENT, pos);
    size_t len = strlen(id) + 1;
    expr->data.Identifier.id = Hxs_strdup(arena, id);

    return expr;
}

HxsExpr *Hxs_Expr_makeField(HxsArena* arena, HxsPosition pos, HxsExpr *parent, char *field)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(arena, HXS_EXPR_FIELD, pos);
    expr->data.Field.obj = parent;
    size_t len = strlen(field) + 1;
    expr->data.Field.field = Hxs_strdup(arena, field);

    return expr;
}

HxsExpr *Hxs_Expr_makeIndex(HxsArena* arena, HxsPosition pos, HxsExpr *parent, HxsExpr *index)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(arena, HXS_EXPR_INDEX_ACCESS, pos);
    expr->data.Index.obj = parent;
    expr->data.Index.index = index;

    return expr;
}

HxsExpr *Hxs_Expr_makeBinop(HxsArena* arena, HxsPosition pos, HxsBinop binop, HxsExpr *left, HxsExpr *right)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(arena, HXS_EXPR_BINOP, pos);
    expr->data.Binop.op = binop;
    expr->data.Binop.left = left;
    expr->data.Binop.right = right;

    return expr;
}

HxsExpr *Hxs_Expr_makeUnop(HxsArena* arena, HxsPosition pos, HxsUnop unop, HxsExpr *value)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(arena, HXS_EXPR_UNOP, pos);
    expr->data.Unop.op = unop;
    expr->data.Unop.expr = value;

    return expr;
}

HxsExpr *Hxs_Expr_makeArrayDeclaration(HxsArena* arena, HxsPosition pos, size_t size, HxsExpr **values)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(arena, HXS_EXPR_ARRAY_DECL, pos);
    expr->data.ArrayDeclaration.size = size;
    expr->data.ArrayDeclaration.body = values;

    return expr;
}

HxsExpr *Hxs_Expr_makeObjDeclaration(HxsArena* arena, HxsPosition pos, size_t size, HxsObjField **values)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(arena, HXS_EXPR_OBJECT_DECL, pos);
    expr->data.ObjectDeclaration.size = size;
    expr->data.ObjectDeclaration.values = values;

    return expr;
}

HxsExpr *Hxs_Expr_makeCall(HxsArena* arena, HxsPosition pos, HxsExpr *target, size_t size, HxsExpr **values)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(arena, HXS_EXPR_CALL, pos);
    expr->data.Call.target = target;
    expr->data.Call.totalArgs = size;
    expr->data.Call.args = values;

    return expr;
}

HxsExpr *Hxs_Expr_makeNew(HxsArena* arena, HxsPosition pos, char *target, size_t size, HxsExpr **values)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(arena, HXS_EXPR_NEW, pos);
    expr->data.New.target = target;
    expr->data.New.totalArgs = size;
    expr->data.New.args = values;

    return expr;
}

HxsExpr *Hxs_Expr_makeBlock(HxsArena* arena, HxsPosition pos, size_t size, HxsExpr **body)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(arena, HXS_EXPR_BLOCK, pos);
    expr->data.Block.size = size;
    expr->data.Block.body = body;

    return expr;
}

HxsExpr *Hxs_Expr_makeIf(HxsArena* arena, HxsPosition pos, HxsExpr *cond, HxsExpr *body, HxsExpr *elsee)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(arena, HXS_EXPR_IF, pos);
    expr->data.If.cond = cond;
    expr->data.If.value = body;
    expr->data.If.elsee = elsee;

    return expr;
}

HxsExpr *Hxs_Expr_makeTernary(HxsArena* arena, HxsPosition pos, HxsExpr *cond, HxsExpr *onTrue, HxsExpr *onFalse)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(arena, HXS_EXPR_TERNARY, pos);
    expr->data.Ternary.cond = cond;
    expr->data.Ternary.onTrue = onTrue;
    expr->data.Ternary.onFalse = onFalse;

    return expr;
}

HxsExpr *Hxs_Expr_makeSwitch(HxsArena* arena, HxsPosition pos, HxsExpr *target)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(arena, HXS_EXPR_SWITCH, pos);
    expr->data.Switch.target = target;

    return expr;
}

HxsExpr *Hxs_Expr_makeFor(HxsArena* arena, HxsPosition pos, char *variable, HxsExpr *target, HxsExpr *body)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(arena, HXS_EXPR_FOR, pos);
    size_t len = strlen(variable) + 1;
    expr->data.For.variable = Hxs_strdup(arena, variable);
    expr->data.For.target = target;
    expr->data.For.body = body;

    return expr;
}

HxsExpr *Hxs_Expr_makeWhile(HxsArena* arena, HxsPosition pos, HxsExpr *cond, HxsExpr *body)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(arena, HXS_EXPR_WHILE, pos);
    expr->data.While.cond = cond;
    expr->data.While.body = body;

    return expr;
}

HxsExpr *Hxs_Expr_makeDoWhile(HxsArena* arena, HxsPosition pos, HxsExpr *cond, HxsExpr *body)
{
    HxsExpr *expr = Hxs_Expr_makeExpression(arena, HXS_EXPR_DO_WHILE, pos);
    expr->data.DoWhile.cond = cond;
    expr->data.DoWhile.body = body;

    return expr;
}