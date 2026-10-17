#include "libavutil/map.h"
#include "libavutil/hash.h"
#include "libavutil/log.h"
#include "libavutil/mem.h"
#include "libavutil/tree.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

Map *create_map(size_t capacity) {
  int leaf_node_len = capacity;
  int internal_node_len = capacity - 1;
  int total_node_len = leaf_node_len + internal_node_len;
  Arena *slot_arena = create_arena(sizeof(TreeSlot[total_node_len]));
  if (slot_arena == NULL) {
    log_oom();
    abort();
  }

  TreeSlot *root_node = arena_alloc(slot_arena, sizeof(TreeSlot));
  root_node->type = TREE_NODE;
  root_node->node.kind = BALANCED_TREE_NODE;
  root_node->node.midpoint = UINT32_MAX / 2;
  root_node->node.slot[0] = NULL;
  root_node->node.slot[1] = NULL;

  Map *map = malloc(sizeof(Map));
  if (map == NULL) {
    log_oom();
    abort();
  }

  map->arena = slot_arena;
  map->root = root_node;

  return map;
}

void destroy_map(Map *map) {
  arena_free(map->arena);
  free(map);
}

void map_insert(void *key, void *value, size_t key_size, Map *map) {
  uint32_t hashed_key = murmurHash3(10, key, key_size);
  TreeSlot *leaf = arena_alloc(map->arena, sizeof(TreeSlot));
  if (leaf == NULL) {
    log_oom();
    abort();
  };

  leaf->leaf.value = value;
  leaf->leaf.index = hashed_key;
  leaf->type = TREE_LEAF;
  tree_insert_leaf(map->root, leaf, map->arena);
}

void *map_get(void *key, size_t key_size, Map *map) {
  uint32_t hashed_key = murmurHash3(10, key, key_size);
  TreeSlot *leaf = tree_find_leaf(map->root, hashed_key);
  if (leaf == NULL) {
    return NULL;
  }

  return leaf->leaf.value;
}
