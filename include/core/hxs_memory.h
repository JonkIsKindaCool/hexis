#pragma once

#include "core/hxs_arena.h"
#include <stddef.h>

void Hxs_memoryInit(size_t arena_size);
void Hxs_memoryShutdown(void);
void Hxs_memoryReset(void);
void* Hxs_alloc(size_t size);
char* Hxs_strdup(const char* str);
