#include "tree.h"
#include "libavutil/log.h"
#include "libavutil/mem.h"
#include <stdint.h>
#include <stdlib.h>

void *tree_slot_value(TreeSlot *slot) {
  if (slot->type == TREE_LEAF)
    return slot->leaf.value;

  return slot->node.value;
}

Path *find_path(uint32_t target_index, TreeSlot *root) {
  Path *path = malloc(sizeof(*path));
  if (path == NULL) {
    log_oom();
    abort();
  }

  path->steps = malloc(sizeof(uint8_t[255]));
  if (path->steps == NULL) {
    free(path);
    log_oom();
    abort();
  }

  path->length = 0;

  TreeSlot *current_node = root;

  while (current_node->type == TREE_NODE) {
    bool target_found = false;

    for (int child_index = 0; child_index < 2; ++child_index) {
      uint32_t *child_indices = current_node->node.index_map[child_index];

      int child_index_count = current_node->node.index_map_len[child_index];

      bool target_in_child = false;

      for (int i = 0; i < child_index_count; ++i) {
        if (child_indices[i] == target_index) {
          target_in_child = true;
          break;
        }
      }

      if (!target_in_child) {
        continue;
      }

      path->steps[path->length++] = child_index;
      current_node = current_node->node.slot[child_index];

      target_found = true;
      break;
    }

    if (!target_found) {
      free_path(path);
      return NULL;
    }
  }

  return path;
}

void free_path(Path *path) {
  free(path->steps);
  free(path);
}

TreeSlot *tree_find_leaf(TreeSlot *root, uint32_t index) {
  TreeSlot *parent = root;
  if (root->node.kind == BALANCED_TREE_NODE)
    while (true) {
      int child_side = index > parent->node.midpoint ? RIGHT : LEFT;

      TreeSlot *child = parent->node.slot[child_side];

      if (child == NULL || child->type == TREE_LEAF) {
        if (child == NULL || child->leaf.index != index) {
          return NULL;
        }

        return child;
      }

      parent = child;
    }
  else
    return NULL;
  // TODO: Implement leaf lookup for unbalanced tree nodes.
}

void tree_insert_leaf(TreeSlot *root, TreeSlot *leaf, Arena *arena) {
  TreeSlot *parent = root;
  int child_side;

  while (true) {
    child_side = leaf->leaf.index > parent->node.midpoint ? RIGHT : LEFT;

    TreeSlot *child = parent->node.slot[child_side];

    if (child == NULL || child->type == TREE_LEAF) {
      if (child == NULL) {
        parent->node.slot[child_side] = leaf;
        return;
      }

      // Preserve the existing leaf slot as the new internal node.
      TreeSlot *new_parent = child;
      TreeSlot *existing_leaf = arena_alloc(arena, sizeof(*existing_leaf));
      *existing_leaf = *new_parent;

      TreeSlot *lower_leaf;
      TreeSlot *higher_leaf;

      if (leaf->leaf.index < existing_leaf->leaf.index) {
        lower_leaf = leaf;
        higher_leaf = existing_leaf;
      } else {
        lower_leaf = existing_leaf;
        higher_leaf = leaf;
      }

      uint32_t index_diff = higher_leaf->leaf.index - lower_leaf->leaf.index;

      new_parent->node.slot[LEFT] = lower_leaf;
      new_parent->node.slot[RIGHT] = higher_leaf;
      new_parent->node.midpoint = lower_leaf->leaf.index + index_diff / 2;
      new_parent->type = TREE_NODE;

      return;
    }

    parent = child;
  }

  // Balancing is deferred until profiling identifies a bottleneck.
}
