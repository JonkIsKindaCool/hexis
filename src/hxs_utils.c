#include "hxs_utils.h"

StringBuffer *init_StringBuffer()
{
    StringBuffer *buf = malloc(sizeof(StringBuffer));
    buf->buf = malloc(0);
    buf->capacity = 0;
    buf->size = 0;

    return buf;
}
void add_char_to_buffer(StringBuffer *buffer, char c)
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
void add_string_buffer(StringBuffer *buffer, const char *str)
{
    int len = strlen(str);

    for (size_t i = 0; i < len; i++)
    {
        add_char_to_buffer(buffer, str[i]);
    }
}
char *buffer_to_str(StringBuffer *buffer)
{
    return buffer->buf;
}

void freeBuffer(StringBuffer *buffer)
{
    free(buffer->buf);
    free(buffer);
}