#include "core/hxs_arena.h"

#include <stdlib.h>
#include <string.h>

#define HXS_ARENA_ALIGNMENT 8

static size_t align_forward(size_t ptr)
{
    size_t mod = ptr & (HXS_ARENA_ALIGNMENT - 1);

    if (mod != 0)
        ptr += HXS_ARENA_ALIGNMENT - mod;

    return ptr;
}

static HxsArenaChunk* create_chunk(size_t size)
{
    HxsArenaChunk* chunk = malloc(sizeof(HxsArenaChunk));

    if (!chunk)
        return NULL;

    chunk->memory = malloc(size);

    if (!chunk->memory)
    {
        free(chunk);
        return NULL;
    }

    chunk->capacity = size;
    chunk->offset = 0;
    chunk->next = NULL;

    return chunk;
}

HxsArena* Hxs_Arena_create(size_t chunkSize)
{
    HxsArena* arena = malloc(sizeof(HxsArena));

    if (!arena)
        return NULL;

    HxsArenaChunk* chunk = create_chunk(chunkSize);

    if (!chunk)
    {
        free(arena);
        return NULL;
    }

    arena->head = chunk;
    arena->current = chunk;
    arena->defaultChunkSize = chunkSize;

    return arena;
}

static bool grow(HxsArena* arena, size_t minSize)
{
    size_t newSize = arena->defaultChunkSize;

    if (minSize > newSize)
        newSize = minSize * 2;

    HxsArenaChunk* chunk = create_chunk(newSize);

    if (!chunk)
        return false;

    arena->current->next = chunk;
    arena->current = chunk;

    return true;
}

void* Hxs_Arena_alloc(HxsArena* arena, size_t size)
{
    if (!arena || size == 0)
        return NULL;

    HxsArenaChunk* chunk = arena->current;

    size_t aligned = align_forward(chunk->offset);

    if (aligned + size > chunk->capacity)
    {
        if (!grow(arena, size))
            return NULL;

        chunk = arena->current;
        aligned = 0;
    }

    void* ptr = chunk->memory + aligned;

    chunk->offset = aligned + size;

    return ptr;
}

void* Hxs_Arena_alloc_zeroed(HxsArena* arena, size_t size)
{
    void* ptr = Hxs_Arena_alloc(arena, size);

    if (ptr)
        memset(ptr, 0, size);

    return ptr;
}

char* Hxs_Arena_strdup(HxsArena* arena, const char* str)
{
    size_t len = strlen(str) + 1;

    char* copy = Hxs_Arena_alloc(arena, len);

    if (!copy)
        return NULL;

    memcpy(copy, str, len);

    return copy;
}

void Hxs_Arena_reset(HxsArena* arena)
{
    if (!arena)
        return;

    HxsArenaChunk* chunk = arena->head;

    while (chunk)
    {
        chunk->offset = 0;
        chunk = chunk->next;
    }

    arena->current = arena->head;
}

void Hxs_Arena_destroy(HxsArena* arena)
{
    if (!arena)
        return;

    HxsArenaChunk* chunk = arena->head;

    while (chunk)
    {
        HxsArenaChunk* next = chunk->next;

        free(chunk->memory);
        free(chunk);

        chunk = next;
    }

    free(arena);
}