#include "types/hxs_types.h"
#include "core/hxs_utils.h"
#include "runtime/hxs_data.h"

#include <stdlib.h>

HxsAstType *Hxs_Type_makeType(HxsArena* arena, HxsAstTypeKind kind)
{
    HxsAstType *type = Hxs_Arena_alloc(arena, sizeof(HxsAstType));
    if (!type)
    {
        return NULL;
    }

    type->kind = kind;
    return type;
}

HxsAstType *Hxs_Type_makeBasic(HxsArena* arena, const char *name, size_t size, HxsAstType **generics)
{
    HxsAstType *type = Hxs_Type_makeType(arena, HXS_BASIC_TYPE);

    if (!type)
    {
        return NULL;
    }

    type->data.basic.name = Hxs_strdup(arena, name);
    type->data.basic.amountOfGenerics = size;
    type->data.basic.generics = generics;

    return type;
}

HxsAstType *Hxs_Type_makeFuncType(HxsArena* arena, size_t size, HxsAstType **args)
{
    HxsAstType *type = Hxs_Type_makeType(arena, HXS_FUNCTION_TYPE);

    if (!type)
    {
        return NULL;
    }

    type->data.function.amountOfArgs = size;
    type->data.function.args = args;

    return type;
}

HxsAstType *Hxs_Type_makeAnonType(HxsArena* arena, size_t size, HxsAnonField **fields)
{
    HxsAstType *type = Hxs_Type_makeType(arena, HXS_ANON_TYPE);

    if (!type)
    {
        return NULL;
    }

    type->data.anon_obj.amountOfFields = size;
    type->data.anon_obj.fields = fields;

    return type;
}