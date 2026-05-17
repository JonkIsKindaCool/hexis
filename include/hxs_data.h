#pragma once
#include <stdbool.h>
#include <stdlib.h>

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

HxsFuncArg*   Hxs_makeFuncArg(bool optional, const char* name, HxsExpr* def, HxsAstType* type);
HxsObjField*  Hxs_makeObjField(const char* name, HxsExpr* value);
HxsAnonField* Hxs_makeAnonField(bool optional, const char* name, HxsAstType* type);
HxsPosition*  Hxs_makePosition(const char* filename, int line, int column, int pos, int length);

void Hxs_freeFuncArg(HxsFuncArg* arg);
void Hxs_freeObjField(HxsObjField* field);
void Hxs_freeAnonField(HxsAnonField* arg);
void Hxs_freePosition(HxsPosition* pos);