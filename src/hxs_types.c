#include "hxs_types.h"

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