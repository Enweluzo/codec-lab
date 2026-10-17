#ifndef AV_MAP_H
#define AV_MAP_H
#include "libavutil/mem.h"
#include "libavutil/tree.h"
#include <stddef.h>

typedef struct {
  Arena *arena;
  TreeSlot *root;
} Map;

extern Map *create_map(size_t capacity);

extern void destroy_map(Map *map);

extern void map_insert(void *key, void *value, size_t key_size, Map *map);

extern void *map_get(void *key, size_t key_size, Map *map);

#endif
