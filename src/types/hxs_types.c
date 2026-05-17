#include "types/hxs_types.h"
#include "runtime/hxs_data.h"
#include "core/hxs_memory.h"

#include <stdlib.h>

HxsAstType *Hxs_Type_makeType(HxsAstTypeKind kind)
{
    HxsAstType *type = Hxs_alloc(sizeof(HxsAstType));
    if (!type)
    {
        return NULL;
    }

    type->kind = kind;
    return type;
}

HxsAstType *Hxs_Type_makeBasic(const char *name, size_t size, HxsAstType **generics)
{
    HxsAstType *type = Hxs_Type_makeType(HXS_BASIC_TYPE);

    if (!type)
    {
        return NULL;
    }

    type->data.basic.name = Hxs_strdup(name);
    type->data.basic.amountOfGenerics = size;
    type->data.basic.generics = generics;

    return type;
}

HxsAstType *Hxs_Type_makeFuncType(size_t size, HxsAstType **args)
{
    HxsAstType *type = Hxs_Type_makeType(HXS_FUNCTION_TYPE);

    if (!type)
    {
        return NULL;
    }

    type->data.function.amountOfArgs = size;
    type->data.function.args = args;

    return type;
}

HxsAstType *Hxs_Type_makeAnonType(size_t size, HxsAnonField **fields)
{
    HxsAstType *type = Hxs_Type_makeType(HXS_ANON_TYPE);

    if (!type)
    {
        return NULL;
    }

    type->data.anon_obj.amountOfFields = size;
    type->data.anon_obj.fields = fields;

    return type;
}

void Hxs_freeType(HxsAstType *type)
{
    if (!type)
    {
        return;
    }

    switch (type->kind)
    {
        case HXS_BASIC_TYPE:
        {
            size_t generic_count = type->data.basic.amountOfGenerics;

            if (type->data.basic.generics)
            {
                for (size_t i = 0; i < generic_count; ++i)
                {
                    Hxs_freeType(type->data.basic.generics[i]);
                }

                free(type->data.basic.generics);
            }

            break;
        }

        case HXS_FUNCTION_TYPE:
        {
            size_t arg_count = type->data.function.amountOfArgs;

            if (type->data.function.args)
            {
                for (size_t i = 0; i < arg_count; ++i)
                {
                    Hxs_freeType(type->data.function.args[i]);
                }

                free(type->data.function.args);
            }

            break;
        }

        case HXS_ANON_TYPE:
        {
            size_t field_count = type->data.anon_obj.amountOfFields;

            if (type->data.anon_obj.fields)
            {
                for (size_t i = 0; i < field_count; ++i)
                {
                    Hxs_freeAnonField(type->data.anon_obj.fields[i]);
                }

                free(type->data.anon_obj.fields);
            }

            break;
        }
    }
}
