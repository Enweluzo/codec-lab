#include "libavutil/mem.h"
#include <stddef.h>
#include <stdlib.h>

Arena *create_arena(size_t capacity) {
  Arena *arena = malloc(sizeof(Arena));
  if (arena == NULL) {
    return NULL;
  }
  arena->buffer = malloc(capacity);
  if (arena->buffer == NULL) {
    return NULL;
  }
  arena->capacity = capacity;
  arena->offset = 0;
  return arena;
}

void *arena_alloc(Arena *arena, size_t size) {
  if (arena->offset + size > arena->capacity) {
    // Increase the capacity
    size_t new_capacity = arena->offset + size;
    arena->buffer = realloc(arena->buffer, new_capacity);
    if (arena->buffer == NULL) {
      return NULL;
    }

    arena->capacity = new_capacity;
  }

  void *ptr = &arena->buffer[arena->offset];
  arena->offset += size;
  return ptr;
}

void arena_free(Arena *arena) {
  free(arena->buffer);
  free(arena);
}
