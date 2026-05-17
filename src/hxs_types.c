#include "hxs_types.h"

HxsAstType *Hxs_Type_makeType(HxsAstTypeKind kind)
{
    HxsAstType *type = malloc(sizeof(HxsAstType));
    type->kind = kind;

    return type;
}
HxsAstType *Hxs_Type_makeBasic(char *name, size_t size, HxsAstType **generics)
{
    HxsAstType *type = Hxs_Type_makeType(HXS_BASIC_TYPE);

    type->data.basic.name = malloc(sizeof(char) * (strlen(name) + 1));
    memcpy(type->data.basic.name, name, sizeof(char) * (strlen(name) + 1));

    type->data.basic.amountOfGenerics = size;
    type->data.basic.generics = generics;

    return type;
}
HxsAstType *Hxs_Type_makeFuncType(size_t size, HxsAstType **args)
{
    HxsAstType *type = Hxs_Type_makeType(HXS_FUNCTION_TYPE);

    type->data.function.amountOfArgs = size;
    type->data.function.args = args;

    return type;
}
HxsAstType *Hxs_Type_makeAnonType(size_t size, HxsAstType **fields)
{
    HxsAstType *type = Hxs_Type_makeType(HXS_FUNCTION_TYPE);

    type->data.anon_obj.amountOfFields = size;
    type->data.anon_obj.fields = fields;

    return type;
}

void Hxs_freeType(HxsAstType *type)
{
    switch (type->kind)
    {
    case HXS_BASIC_TYPE:
        free(type->data.basic.name);
        size_t size = type->data.basic.amountOfGenerics;

        if (type->data.basic.generics != NULL)
        {
            for (size_t i = 0; i < size; i++)
            {
                Hxs_freeType(type->data.basic.generics[i]);
            }

            free(type->data.basic.generics);
        }

        break;
    case HXS_FUNCTION_TYPE:
        size_t size = type->data.function.amountOfArgs;

        if (type->data.function.args != NULL)
        {
            for (size_t i = 0; i < size; i++)
            {
                Hxs_freeType(type->data.function.args[i]);
            }

            free(type->data.function.args);
        }
        break;
    case HXS_ANON_TYPE:
        size_t size = type->data.anon_obj.amountOfFields;

        if (type->data.anon_obj.fields != NULL)
        {
            for (size_t i = 0; i < size; i++)
            {
                Hxs_freeType(type->data.anon_obj.fields[i]);
            }

            free(type->data.anon_obj.fields);
        }
        break;
    }

    free(type);
}