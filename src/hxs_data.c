#include "hxs_data.h"
#include "hxs_ast.h"   
#include "hxs_types.h"  
#include <string.h>

HxsFuncArg* Hxs_makeFuncArg(bool optional, const char* name, HxsExpr* def, HxsAstType* type)
{
    HxsFuncArg* arg = malloc(sizeof(*arg));
    arg->optional = optional;
    arg->name = name ? strdup(name) : NULL;
    arg->def = def;
    arg->type = type;
    return arg;
}

HxsObjField* Hxs_makeObjField(const char* name, HxsExpr* value)
{
    HxsObjField* f = malloc(sizeof(*f));
    f->name = name ? strdup(name) : NULL;
    f->value = value;
    return f;
}

HxsAnonField* Hxs_makeAnonField(bool optional, const char* name, HxsAstType* type)
{
    HxsAnonField* f = malloc(sizeof(*f));
    f->optional = optional;
    f->name = name ? strdup(name) : NULL;
    f->type = type;
    return f;
}

HxsPosition* Hxs_makePosition(const char* filename, int line, int column, int pos, int length)
{
    HxsPosition* p = malloc(sizeof(*p));
    p->filename = strdup(filename ? filename : "<unknown>");
    p->line = line;
    p->column = column;
    p->pos = pos;
    p->length = length;
    return p;
}

void Hxs_freeFuncArg(HxsFuncArg* arg)
{
    if (!arg) return;
    free(arg->name);
    Hxs_free_Expr(arg->def);
    Hxs_freeType(arg->type);
    free(arg);
}

void Hxs_freeObjField(HxsObjField* field)
{
    if (!field) return;
    free(field->name);
    Hxs_free_Expr(field->value);
    free(field);
}

void Hxs_freeAnonField(HxsAnonField* arg)
{
    if (!arg) return;
    free(arg->name);
    Hxs_freeType(arg->type);
    free(arg);
}

void Hxs_freePosition(HxsPosition* pos)
{
    if (!pos) return;
    free(pos->filename);
    free(pos);
}