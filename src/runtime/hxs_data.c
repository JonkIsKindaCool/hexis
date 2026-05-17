#include "runtime/hxs_data.h"
#include "ast/hxs_ast.h"     
#include "types/hxs_types.h"  
#include <string.h>

HxsFuncArg* Hxs_makeFuncArg(HxsArena* arena, bool optional, const char* name, HxsExpr* def, HxsAstType* type)
{
    HxsFuncArg* arg = Hxs_Arena_alloc(arena, sizeof(*arg));
    arg->optional = optional;
    arg->name = name ? strdup(name) : NULL;
    arg->def = def;
    arg->type = type;
    return arg;
}

HxsObjField* Hxs_makeObjField(HxsArena* arena, const char* name, HxsExpr* value)
{
    HxsObjField* f = Hxs_Arena_alloc(arena, sizeof(*f));
    f->name = name ? strdup(name) : NULL;
    f->value = value;
    return f;
}

HxsAnonField* Hxs_makeAnonField(HxsArena* arena, bool optional, const char* name, HxsAstType* type)
{
    HxsAnonField* f = Hxs_Arena_alloc(arena, sizeof(*f));
    f->optional = optional;
    f->name = name ? strdup(name) : NULL;
    f->type = type;
    return f;
}

HxsPosition* Hxs_makePosition(HxsArena* arena, const char* filename, int line, int column, int pos, int length)
{
    HxsPosition* p = Hxs_Arena_alloc(arena, sizeof(*p));
    p->filename = strdup(filename ? filename : "<unknown>");
    p->line = line;
    p->column = column;
    p->pos = pos;
    p->length = length;
    return p;
}