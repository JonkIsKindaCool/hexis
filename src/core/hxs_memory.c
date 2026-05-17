#include "core/hxs_memory.h"

#include <stdlib.h>
#include <string.h>

static HxsArena* g_arena = NULL;

void Hxs_memoryInit(size_t arena_size)
{
    if (!g_arena)
    {
        g_arena = Hxs_Arena_create(arena_size);
    }
}

void Hxs_memoryShutdown(void)
{
    if (g_arena)
    {
        Hxs_Arena_destroy(g_arena);
        g_arena = NULL;
    }
}

void Hxs_memoryReset(void)
{
    if (g_arena)
    {
        Hxs_Arena_reset(g_arena);
    }
}

void* Hxs_alloc(size_t size)
{
    if (g_arena)
    {
        void* ptr = Hxs_Arena_alloc(g_arena, size);
        if (ptr)
        {
            memset(ptr, 0, size);
            return ptr;
        }
    }

    return calloc(1, size);
}

char* Hxs_strdup(const char* str)
{
    if (!str)
    {
        return NULL;
    }

    size_t len = strlen(str) + 1;
    char* copy = Hxs_alloc(len);

    if (copy)
    {
        memcpy(copy, str, len);
    }

    return copy;
}
