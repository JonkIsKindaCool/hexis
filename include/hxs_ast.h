#pragma once

typedef enum {
    INT_EXPR,
    FLOAT_EXPR,
    SINGLE_STRING_EXPR,
    DOUBLE_STRING_EXPR,
    BINARY_EXPR,
    NEW_EXPR
} EXPR_KIND;

typedef struct
{
    EXPR_KIND kind;
    union
    {
        struct integer
        {
            /* data */
        };
        
    };
    
} Expr;


typedef enum {
    FUNCTION_STMT,
    IF_STMT,
    WHILE_STMT,
    SWITCH_STMT,
    FOR_STMT,
} STMT_KIND;

typedef struct
{
    STMT_KIND kind;
    union
    {
        struct expr
        {
            
        };
        
    };
    
} Stmt;
