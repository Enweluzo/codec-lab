#ifndef AV_MEM_H
#define AV_MEM_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint8_t *buffer;
  size_t capacity;
  size_t offset;
} Arena;

extern Arena *create_arena(size_t buf_size);

extern void *arena_alloc(Arena *arena, size_t size);

extern void arena_free(Arena *arena);

#endif
