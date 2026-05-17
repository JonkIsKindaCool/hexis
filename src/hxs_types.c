#include "hxs_types.h"
#include "hxs_data.h"

HxsAstType* Hxs_Type_makeType(HxsAstTypeKind kind)
{
    HxsAstType* type = malloc(sizeof(HxsAstType));
    if (type == NULL)
    {
        return NULL;
    }

    type->kind = kind;
    return type;
}

HxsAstType* Hxs_Type_makeBasic(const char* name, size_t size, HxsAstType** generics)
{
    HxsAstType* type = Hxs_Type_makeType(HXS_BASIC_TYPE);
    if (type == NULL)
    {
        return NULL;
    }

    size_t nameLength = strlen(name);
    type->data.basic.name = malloc(nameLength + 1);

    if (type->data.basic.name != NULL)
    {
        memcpy(type->data.basic.name, name, nameLength + 1);
    }

    type->data.basic.amountOfGenerics = size;
    type->data.basic.generics = generics;

    return type;
}

HxsAstType* Hxs_Type_makeFuncType(size_t size, HxsAstType** args)
{
    HxsAstType* type = Hxs_Type_makeType(HXS_FUNCTION_TYPE);
    if (type == NULL)
    {
        return NULL;
    }

    type->data.function.amountOfArgs = size;
    type->data.function.args = args;

    return type;
}

HxsAstType* Hxs_Type_makeAnonType(size_t size, HxsAnonField** fields)
{
    HxsAstType* type = Hxs_Type_makeType(HXS_ANON_TYPE);
    if (type == NULL)
    {
        return NULL;
    }

    type->data.anon_obj.amountOfFields = size;
    type->data.anon_obj.fields = fields;

    return type;
}

void Hxs_freeType(HxsAstType* type)
{
    if (type == NULL)
    {
        return;
    }

    switch (type->kind)
    {
        case HXS_BASIC_TYPE:
        {
            free(type->data.basic.name);

            if (type->data.basic.generics != NULL)
            {
                size_t genericCount = type->data.basic.amountOfGenerics;

                for (size_t i = 0; i < genericCount; ++i)
                {
                    Hxs_freeType(type->data.basic.generics[i]);
                }

                free(type->data.basic.generics);
            }

            break;
        }

        case HXS_FUNCTION_TYPE:
        {
            if (type->data.function.args != NULL)
            {
                size_t argCount = type->data.function.amountOfArgs;

                for (size_t i = 0; i < argCount; ++i)
                {
                    Hxs_freeType(type->data.function.args[i]);
                }

                free(type->data.function.args);
            }

            break;
        }

        case HXS_ANON_TYPE:
        {
            if (type->data.anon_obj.fields != NULL)
            {
                size_t fieldCount = type->data.anon_obj.amountOfFields;

                for (size_t i = 0; i < fieldCount; ++i)
                {
                    Hxs_freeAnonField(type->data.anon_obj.fields[i]);
                }

                free(type->data.anon_obj.fields);
            }

            break;
        }
    }

    free(type);
}
