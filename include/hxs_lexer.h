#pragma once
#include "hxs_token.h"

typedef struct
{
    const char *src;

    int pos;
    int line;
    int linePos;
} HxsLexer;

HxsLexer *make_lexer(const char *src);
void      free_lexer(HxsLexer *lexer);

HxsToken *get_token(HxsLexer *lexer, bool advance);