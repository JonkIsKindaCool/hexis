#pragma once
#include "lexer/hxs_token.h"
#include "core/hxs_arena.h"
#include "setjmp.h"
#include <stdarg.h>

typedef struct
{
    const char *src;

    int pos;
    int line;
    int linePos;

    HxsArena* arena;

    jmp_buf error_jmp;
    char error_msg[512];
    bool has_error;
} HxsLexer;

HxsLexer *Hxs_make_lexer(HxsArena* arena, const char *src);
void Hxs_free_lexer(HxsLexer *lexer);

HxsToken *Hxs_get_token(HxsLexer *lexer, bool advance);
void Hxs_lexer_throw(HxsLexer *lexer, const char *fmt, ...);