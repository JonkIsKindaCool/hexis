#pragma once
#include "lexer/hxs_lexer.h"
#include "core/hxs_utils.h"
#include "types/hxs_types.h"
#include "runtime/hxs_data.h"

typedef struct HxsExpr HxsExpr;
typedef struct HxsPosition HxsPosition;
typedef struct HxsObjField HxsObjField;

typedef enum
{
    HXS_EXPR_CONST_INT,
    HXS_EXPR_CONST_FLOAT,
    HXS_EXPR_CONST_STRING,
    HXS_EXPR_CONST_BOOL,
    HXS_EXPR_CONST_NULL,

    HXS_EXPR_IDENT,
    HXS_EXPR_FIELD,
    HXS_EXPR_INDEX_ACCESS,

    HXS_EXPR_BINOP,
    HXS_EXPR_UNOP,

    HXS_EXPR_ARRAY_DECL,
    HXS_EXPR_OBJECT_DECL,

    HXS_EXPR_CALL,
    HXS_EXPR_NEW,

    HXS_EXPR_BLOCK,
    HXS_EXPR_IF,
    HXS_EXPR_TERNARY, // cond ? expr1 : expr2
    HXS_EXPR_SWITCH,

    HXS_EXPR_FOR,
    HXS_EXPR_WHILE,
    HXS_EXPR_DO_WHILE,

    HXS_EXPR_FUNCTION,
    HXS_EXPR_RETURN,

    HXS_EXPR_BREAK,
    HXS_EXPR_CONTINUE,
    HXS_EXPR_THROW,
    HXS_EXPR_TRY,

    HXS_EXPR_CAST, // cast expr or cast(expr, Type) or (expr : Type)
    HXS_EXPR_IS,   // expr is Type

    HXS_EXPR_PARENTHESIS,
    HXS_EXPR_UNTYPED,
    HXS_EXPR_META,

    HXS_EXPR_ASSIGN,
    HXS_EXPR_ASSIGN_OP,

    HXS_EXPR_VAR_DECL

} HxsExprKind;

typedef enum
{
    HXS_BINOP_ADD, // +
    HXS_BINOP_SUB, // -
    HXS_BINOP_MUL, // *
    HXS_BINOP_DIV, // /
    HXS_BINOP_MOD, // %

    HXS_BINOP_ASSIGN_ADD, // +=
    HXS_BINOP_ASSIGN_SUB, // -=
    HXS_BINOP_ASSIGN_MUL, // *=
    HXS_BINOP_ASSIGN_DIV, // /=
    HXS_BINOP_ASSIGN_MOD, // %=

    HXS_BINOP_EQ,  // ==
    HXS_BINOP_NEQ, // !=
    HXS_BINOP_LT,  // <
    HXS_BINOP_LE,  // <=
    HXS_BINOP_GT,  // >
    HXS_BINOP_GE,  // >=

    HXS_BINOP_AND, // &&
    HXS_BINOP_OR,  // ||

    HXS_BINOP_BIT_AND, // &
    HXS_BINOP_BIT_OR,  // |
    HXS_BINOP_BIT_XOR, // ^
    HXS_BINOP_SHL,     // <<
    HXS_BINOP_SHR,     // >>
    HXS_BINOP_USHR,    // >>>

    HXS_BINOP_IN,
    HXS_BINOP_SPREAD,

} HxsBinop;

typedef enum
{
    HXS_UNOP_NEG,      // -expr
    HXS_UNOP_NOT,      // !expr
    HXS_UNOP_BIT_NOT,  // ~expr
    HXS_UNOP_PRE_INC,  // ++expr
    HXS_UNOP_PRE_DEC,  // --expr
    HXS_UNOP_POST_INC, // expr++
    HXS_UNOP_POST_DEC, // expr--
} HxsUnop;

struct HxsExpr
{
    HxsExprKind kind;
    HxsPosition pos;
    union
    {
        struct
        {
            int64_t value;
        } ConstInt;
        struct
        {
            double value;
        } ConstFloat;
        struct
        {
            char *value;
        } ConstString;
        struct
        {
            bool value;
        } ConstBool;
        struct
        {
            char *id;
        } Identifier;
        struct
        {
            HxsExpr *obj;
            char *field;
        } Field;
        struct
        {
            HxsExpr *obj;
            HxsExpr *index;
        } Index;
        struct
        {
            HxsExpr *left;
            HxsBinop op;
            HxsExpr *right;
        } Binop;
        struct
        {
            HxsExpr *expr;
            HxsUnop op;
        } Unop;
        struct
        {
            size_t size;
            HxsExpr **body;
        } ArrayDeclaration;
        struct
        {
            size_t size;
            HxsObjField **values;
        } ObjectDeclaration;
        struct
        {
            HxsExpr *target;
            size_t totalArgs;
            HxsExpr **args;
        } Call;
        struct
        {
            char *target;
            size_t totalArgs;
            HxsExpr **args;
        } New;
        struct
        {
            size_t size;
            HxsExpr **body;
        } Block;
        struct
        {
            HxsExpr *cond;
            HxsExpr *value;
            HxsExpr *elsee;
        } If;
        struct
        {
            HxsExpr *cond;
            HxsExpr *onTrue;
            HxsExpr *onFalse;
        } Ternary;
        struct
        {
            HxsExpr *target;
        } Switch;
        struct
        {
            char *variable;
            HxsExpr *target;
            HxsExpr *body;
        } For;
        struct
        {
            HxsExpr *cond;
            HxsExpr *body;
        } While;
        struct
        {
            HxsExpr *cond;
            HxsExpr *body;
        } DoWhile;

        struct
        {
            bool isConst;
            char *name;
            HxsAstType *type;
            HxsExpr *value;
        } VarDecl;
        
    } data;
};

HxsExpr *Hxs_Expr_makeExpression(HxsArena* arena, HxsExprKind kind, HxsPosition pos);

HxsExpr *Hxs_Expr_makeIntLiteral(HxsArena* arena, HxsPosition pos, int64_t value);
HxsExpr *Hxs_Expr_makeFloatLiteral(HxsArena* arena, HxsPosition pos, double value);
HxsExpr *Hxs_Expr_makeStringLiteral(HxsArena* arena, HxsPosition pos, char *value);
HxsExpr *Hxs_Expr_makeBooleanLiteral(HxsArena* arena, HxsPosition pos, bool value);

HxsExpr *Hxs_Expr_makeIdentifier(HxsArena* arena, HxsPosition pos, char *id);

HxsExpr *Hxs_Expr_makeField(HxsArena* arena, HxsPosition pos, HxsExpr *parent, char *field);
HxsExpr *Hxs_Expr_makeIndex(HxsArena* arena, HxsPosition pos, HxsExpr *parent, HxsExpr *index);

HxsExpr *Hxs_Expr_makeBinop(HxsArena* arena, HxsPosition pos, HxsBinop binop, HxsExpr *left, HxsExpr *right);
HxsExpr *Hxs_Expr_makeUnop(HxsArena* arena, HxsPosition pos, HxsUnop unop, HxsExpr *value);

HxsExpr *Hxs_Expr_makeArrayDeclaration(HxsArena* arena, HxsPosition pos, size_t size, HxsExpr **values);
HxsExpr *Hxs_Expr_makeObjDeclaration(HxsArena* arena, HxsPosition pos, size_t size, HxsObjField **values);

HxsExpr *Hxs_Expr_makeCall(HxsArena* arena, HxsPosition pos, HxsExpr *target, size_t size, HxsExpr **values);
HxsExpr *Hxs_Expr_makeNew(HxsArena* arena, HxsPosition pos, char *target, size_t size, HxsExpr **values);

HxsExpr *Hxs_Expr_makeBlock(HxsArena* arena, HxsPosition pos, size_t size, HxsExpr **body);
HxsExpr *Hxs_Expr_makeIf(HxsArena* arena, HxsPosition pos, HxsExpr *cond, HxsExpr *body, HxsExpr *elsee);
HxsExpr *Hxs_Expr_makeTernary(HxsArena* arena, HxsPosition pos, HxsExpr *cond, HxsExpr *onTrue, HxsExpr *onFalse);

HxsExpr *Hxs_Expr_makeSwitch(HxsArena* arena, HxsPosition pos, HxsExpr *target);
HxsExpr *Hxs_Expr_makeFor(HxsArena* arena, HxsPosition pos, char *variable, HxsExpr *target, HxsExpr *body);
HxsExpr *Hxs_Expr_makeWhile(HxsArena* arena, HxsPosition pos, HxsExpr *cond, HxsExpr *body);
HxsExpr *Hxs_Expr_makeDoWhile(HxsArena* arena, HxsPosition pos, HxsExpr *cond, HxsExpr *body);

HxsExpr *Hxs_Expr_makeVar(HxsArena* arena, HxsPosition pos, bool isConst, char *name, HxsAstType *type, HxsExpr *value);