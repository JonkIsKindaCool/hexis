#pragma once
#include <stdlib.h>
#include <string.h>

typedef struct HxsAnonField HxsAnonField; 
typedef struct HxsAstType HxsAstType;

typedef enum
{
    HXS_BASIC_TYPE,
    HXS_FUNCTION_TYPE,
    HXS_ANON_TYPE,
} HxsAstTypeKind;

struct HxsAstType
{
    HxsAstTypeKind kind;

    union
    {
        struct
        {
            char *name;
            size_t amountOfGenerics;
            HxsAstType** generics;
        } basic;
        
        struct 
        {
            size_t amountOfArgs;
            HxsAstType** args;
        } function;

        struct
        {
            size_t amountOfFields;
            HxsAnonField** fields;
        } anon_obj;
    } data;
};

HxsAstType* Hxs_Type_makeType(HxsAstTypeKind kind);
HxsAstType* Hxs_Type_makeBasic(const char* name, size_t size, HxsAstType** generics);
HxsAstType* Hxs_Type_makeFuncType(size_t size, HxsAstType** args);
HxsAstType* Hxs_Type_makeAnonType(size_t size, HxsAnonField** fields);

void Hxs_freeType(HxsAstType* type);