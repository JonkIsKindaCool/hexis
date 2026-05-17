#pragma once
#include "hxs_ast.h"

typedef struct HxsFuncArg HxsFuncArg;
typedef struct HxsAnonField HxsAnonField;
typedef struct HxsObjField HxsObjField;

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

void Hxs_freeObjField(HxsObjField *field);
void Hxs_freeFuncArg(HxsFuncArg* arg);
void Hxs_freeAnonField(HxsAnonField* arg);
void Hxs_freePosition(HxsPosition *pos);