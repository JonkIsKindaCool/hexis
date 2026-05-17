#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct HxsArenaChunk {
    uint8_t* memory;
    size_t capacity;
    size_t offset;

    struct HxsArenaChunk* next;
} HxsArenaChunk;

typedef struct {
    HxsArenaChunk* head;
    HxsArenaChunk* current;

    size_t defaultChunkSize;
} HxsArena;

HxsArena* Hxs_Arena_create(size_t chunkSize);

void* Hxs_Arena_alloc(HxsArena* arena, size_t size);
void* Hxs_Arena_alloc_zeroed(HxsArena* arena, size_t size);

char* Hxs_Arena_strdup(HxsArena* arena, const char* str);

void Hxs_Arena_reset(HxsArena* arena);
void Hxs_Arena_destroy(HxsArena* arena);
