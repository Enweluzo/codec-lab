#ifndef TREE_H
#define TREE_H
#include "libavutil/mem.h"
#include <stdint.h>

#define LEFT 0
#define RIGHT 1

typedef struct TreeSlot TreeSlot;

typedef struct TreeLeaf {
  uint32_t index;
  void *value;
} TreeLeaf;

typedef struct TreeNode {
  // Each branch points to a child node or a leaf (see TreeSlot).
  TreeSlot *slot[2];

  // Caller-owned metadata.
  void *value;

  // Selects the active member of the union below.
  enum {
    UNBALANCED_TREE_NODE,
    BALANCED_TREE_NODE,
  } kind;

  union {
    // UNBALANCED_TREE_NODE: explicit, unordered lists of the indexes
    // reachable through each branch. Owned by the node.
    struct {
      uint32_t *index_map[2];
      uint32_t index_map_len[2];
    };
    // BALANCED_TREE_NODE: indexes below midpoint go to slot[0], the
    // rest to slot[1].
    struct {
      uint32_t midpoint;
    };
  };
} TreeNode;

typedef struct TreeSlot {
  // Each slot holds either a child node or a leaf.
  enum { TREE_NODE, TREE_LEAF } type;

  union {
    TreeNode node;
    TreeLeaf leaf;
  };
} TreeSlot;

// Returns the value stored in the slot's node or leaf.
extern void *tree_slot_value(TreeSlot *slot);

typedef struct {
  uint8_t *steps;
  int length;
} Path;

extern TreeSlot *tree_find_leaf(TreeSlot *root, uint32_t index);
extern void tree_insert_leaf(TreeSlot *root, TreeSlot *leaf, Arena *arena);

// Returns the root-to-leaf path for the given index (0 = left, 1 = right).
extern Path *find_path(uint32_t index, TreeSlot *root);
extern void free_path(Path *path);

#endif
