#pragma once

#include "string.h"
#include "stdlib.h"

typedef struct
{
    char *buf;
    int size;
    int capacity;
} StringBuffer;

StringBuffer* init_StringBuffer();
void add_char_to_buffer(StringBuffer* buffer, char c);
void add_string_buffer(StringBuffer* buffer, const char* str);
char* buffer_to_str(StringBuffer* buffer);
void freeBuffer(StringBuffer* buffer);