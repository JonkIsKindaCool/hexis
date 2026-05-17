#pragma once
#include <stdbool.h>
#include <stdlib.h>
#include "core/hxs_arena.h"

typedef struct HxsExpr HxsExpr;
typedef struct HxsAstType HxsAstType;

typedef struct HxsFuncArg HxsFuncArg;
typedef struct HxsObjField HxsObjField;
typedef struct HxsAnonField HxsAnonField;
typedef struct HxsPosition HxsPosition;

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
};

struct HxsAnonField
{
    bool optional;
    char *name;
    HxsAstType *type;
};

struct HxsPosition
{
    char *filename;
    int line;
    int column;
    int pos;
    int length;
};

HxsFuncArg*   Hxs_makeFuncArg(HxsArena* arena, bool optional, const char* name, HxsExpr* def, HxsAstType* type);
HxsObjField*  Hxs_makeObjField(HxsArena* arena, const char* name, HxsExpr* value);
HxsAnonField* Hxs_makeAnonField(HxsArena* arena, bool optional, const char* name, HxsAstType* type);
HxsPosition*  Hxs_makePosition(HxsArena* arena, const char* filename, int line, int column, int pos, int length);