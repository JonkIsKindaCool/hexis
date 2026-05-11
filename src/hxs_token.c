#include "hxs_token.h"

HxsToken *create_base_token(HxsTokenKind kind, size_t line, size_t start, size_t end)
{
    HxsToken *tok = malloc(sizeof(HxsToken));
    if (tok == NULL)
        return NULL;

    tok->kind  = kind;
    tok->value = NULL;
    tok->line  = line;
    tok->start = start;
    tok->end   = end;

    return tok;
}

HxsToken *make_int_token(int64_t value, size_t line, size_t start, size_t end)
{
    HxsToken *tok = create_base_token(INT_TOKEN, line, start, end);
    if (tok == NULL)
        return NULL;

    tok->value = (void *)(intptr_t)value; 
    return tok;
}

HxsToken *make_float_token(double value, size_t line, size_t start, size_t end)
{
    HxsToken *tok = create_base_token(FLOAT_TOKEN, line, start, end);
    if (tok == NULL)
        return NULL;

    double *val = malloc(sizeof(double));
    if (val == NULL)
    {
        free(tok);
        return NULL;
    }

    *val = value;
    tok->value = val;

    return tok;
}

HxsToken *make_identifier_token(const char *value, size_t line, size_t start, size_t end)
{
    if (value == NULL)
        return NULL;

    HxsToken *tok = create_base_token(IDENTIFIER_TOKEN, line, start, end);
    if (tok == NULL)
        return NULL;

    size_t len = strlen(value);
    tok->value = malloc(len + 1);

    if (tok->value == NULL)
    {
        free(tok);
        return NULL;
    }

    memcpy(tok->value, value, len + 1); 
    return tok;
}

HxsToken *make_string_token(const char *value, bool single, size_t line, size_t start, size_t end)
{
    if (value == NULL)
        return NULL;

    HxsToken *tok = create_base_token(
        single ? SINGLE_QUOTE_STRING_TOKEN : DOUBLE_QUOTE_STRING_TOKEN,
        line, start, end);

    if (tok == NULL)
        return NULL;

    size_t len = strlen(value);
    tok->value = malloc(len + 1);

    if (tok->value == NULL)
    {
        free(tok);
        return NULL;
    }

    memcpy(tok->value, value, len + 1);

    return tok;
}

void freeToken(HxsToken *token)
{
    if (token == NULL)
        return;

    if (token->kind == IDENTIFIER_TOKEN ||
        token->kind == SINGLE_QUOTE_STRING_TOKEN ||
        token->kind == DOUBLE_QUOTE_STRING_TOKEN ||
        token->kind == FLOAT_TOKEN)
    {
        free(token->value);
    }

    free(token);
}

void print_hxstoken(HxsToken* token){
    char *tokenName = NULL;

}