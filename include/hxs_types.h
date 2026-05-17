#pragma once
#include "string.h"
#include "stdlib.h"

typedef struct HxsAstType HxsAstType;

typedef enum
{
    HXS_BASIC_TYPE,    // variable:Int or variable:Map<String, Int>
    HXS_FUNCTION_TYPE, // variable:(String, Int, Float) => Void
    HXS_ANON_TYPE,     // variable:{id:String, ?value:Int}
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
HxsAstType* Hxs_Type_makeBasic(char* name, size_t size, HxsAstType** generics);
HxsAstType* Hxs_Type_makeFuncType(size_t size, HxsAstType** args);
HxsAstType* Hxs_Type_makeAnonType(size_t size, HxsAstType** fields);

void Hxs_freeType(HxsAstType* type);