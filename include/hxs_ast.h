#pragma once
#include "hxs_lexer.h"

typedef struct HxsExpr HxsExpr;
typedef struct HxsFuncArg HxsFuncArg;
typedef struct HxsAstType HxsAstType;
typedef struct HxsAnonField HxsAnonField;
typedef struct HxsObjField HxsObjField;
typedef struct HxsPosition HxsPosition;

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

typedef enum
{
    HXS_BASIC_TYPE,    // variable:Int or variable:Map<String, Int>
    HXS_FUNCTION_TYPE, // variable:(String, Int, Float) => Void
    HXS_ANON_TYPE,     // variable:{id:String, ?value:Int}
} HxsAstTypeKind;

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
    } data;
};

struct HxsFuncArg
{
    bool optional;
    char *name;
    HxsExpr *def;
    HxsAstType *type;
};
struct HxsObjField
{
    char *name;
    HxsExpr *value;
    HxsAstType *type;
};

struct HxsAstType
{
    HxsAstTypeKind type;
    union
    {
        struct
        {
            char *name;
            size_t genericsLength;
            HxsAstType **generics;
        } Basic;

        struct
        {
            size_t argsLength;
            HxsAstType **argsLength;
            HxsAstType *ret;
        } Function;

        struct
        {
            size_t fieldsLength;
            HxsAnonField **fields;
        } Anon; 
    } data;
};

struct HxsAnonField
{
    bool optional;
    char *name;
    HxsAstType *type;
};

struct HxsPosition
{
    const char *filename;
    int line;
    int column;
    int pos;
    int length;
};
