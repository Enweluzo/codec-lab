#include "libavutil/mem.h"
#include <stddef.h>
#include <stdlib.h>

Arena *create_arena(size_t buf_size) {
  Arena *arena = malloc(sizeof(Arena));
  arena->buffer = malloc(buf_size);
  arena->capacity = buf_size;
  arena->offset = 0;
  return arena;
}

void *arena_alloc(Arena *arena, size_t size) {
  if (arena->offset + size > arena->capacity) {
    return NULL; // Out of memory
  }

  void *ptr = &arena->buffer[arena->offset];
  arena->offset += size;
  return ptr;
}

void arena_free(Arena *arena) {
  free(arena->buffer);
  free(arena);
}
