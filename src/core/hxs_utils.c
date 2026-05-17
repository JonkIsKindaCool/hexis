#include "core/hxs_utils.h"

StringBuffer *Hxs_init_StringBuffer()
{
    StringBuffer *buf = malloc(sizeof(StringBuffer));
    buf->buf = malloc(0);
    buf->capacity = 0;
    buf->size = 0;

    return buf;
}
void Hxs_add_char_to_buffer(StringBuffer *buffer, char c)
{
    buffer->size++;
    if (buffer->size >= buffer->capacity)
    {
        char *new = realloc(buffer->buf, sizeof(char) * (buffer->size + 1));
        if (!new)
            return;
        buffer->buf = new;
        buffer->capacity = buffer->size + 1;
    }
    buffer->buf[buffer->size - 1] = c;
    buffer->buf[buffer->size] = '\0';
}
void Hxs_add_string_buffer(StringBuffer *buffer, const char *str)
{
    int len = strlen(str);

    for (size_t i = 0; i < len; i++)
    {
        Hxs_add_char_to_buffer(buffer, str[i]);
    }
}
char *Hxs_buffer_to_str(StringBuffer *buffer)
{
    return buffer->buf;
}

void Hxs_freeBuffer(StringBuffer *buffer)
{
    free(buffer->buf);
    free(buffer);
}

char *Hxs_strdup(HxsArena* arena, const char *str)
{
    if (!str)
    {
        return NULL;
    }

    size_t len = strlen(str) + 1;
    char *copy = Hxs_Arena_alloc(arena, len);

    if (copy)
    {
        memcpy(copy, str, len);
    }

    return copy;
}
