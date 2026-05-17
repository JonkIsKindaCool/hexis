#ifndef HXS_ARENA_H
#define HXS_ARENA_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t* memory;
    size_t capacity;
    size_t offset;
} HxsArena;

HxsArena* Hxs_Arena_create(size_t size);
void* Hxs_Arena_alloc(HxsArena* arena, size_t size);
void Hxs_Arena_reset(HxsArena* arena);
void Hxs_Arena_destroy(HxsArena* arena);

#endif
