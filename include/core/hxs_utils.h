#pragma once

#include "string.h"
#include "core/hxs_arena.h"
#include "stdlib.h"

typedef struct HxsExpr HxsExpr;

typedef struct
{
    char *buf;
    int size;
    int capacity;
} StringBuffer;

StringBuffer* Hxs_init_StringBuffer();
void Hxs_add_char_to_buffer(StringBuffer* buffer, char c);
void Hxs_add_string_buffer(StringBuffer* buffer, const char* str);
char* Hxs_buffer_to_str(StringBuffer* buffer);
void Hxs_freeBuffer(StringBuffer* buffer);

char *Hxs_strdup(HxsArena* arena, const char *str);

void Hxs_printExpr(HxsExpr *expr);