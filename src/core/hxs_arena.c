#include "../../include/core/hxs_arena.h"

#include <stdlib.h>
#include <string.h>

HxsArena* Hxs_Arena_create(size_t size)
{
    HxsArena* arena = malloc(sizeof(HxsArena));

    if (!arena)
        return NULL;

    arena->memory = malloc(size);

    if (!arena->memory)
    {
        free(arena);
        return NULL;
    }

    arena->capacity = size;
    arena->offset = 0;

    memset(arena->memory, 0, size);

    return arena;
}

void* Hxs_Arena_alloc(HxsArena* arena, size_t size)
{
    if (!arena)
        return NULL;

    if (arena->offset + size > arena->capacity)
        return NULL;

    void* ptr = arena->memory + arena->offset;

    arena->offset += size;

    return ptr;
}

void Hxs_Arena_reset(HxsArena* arena)
{
    if (!arena)
        return;

    arena->offset = 0;
}

void Hxs_Arena_destroy(HxsArena* arena)
{
    if (!arena)
        return;

    free(arena->memory);
    free(arena);
}
