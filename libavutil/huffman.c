#include "libavutil/huffman.h"
#include "libavutil/log.h"
#include "libavutil/mem.h"
#include "libavutil/tree.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

HuffmanCode *get_huffman_code(const void *_src, size_t size) {
  const uint8_t *src = _src;

  int leaf_node_count = 0;
  uint64_t byte_frequency[256] = {0};

  for (size_t i = 0; i < size; ++i) {
    uint8_t byte = src[i];

    if (byte_frequency[byte] == 0) {
      leaf_node_count++;
    }

    byte_frequency[byte]++;
  }

  int internal_node_count = leaf_node_count - 1;
  int total_nodes = internal_node_count + leaf_node_count;

  /*
   * Maximum required storage:
   * - slot_arena: tree slots (leaf and internal nodes)
   * - index_map_arena: merged index maps
   * - node_value_arena: internal node values
   *   (leaf nodes use freq_map directly)
   */
  Arena *slot_arena = create_arena(sizeof(TreeSlot[total_nodes]));

  Arena *index_map_arena =
      create_arena(sizeof(int[internal_node_count * total_nodes]));

  Arena *node_value_arena = create_arena(sizeof(uint64_t[internal_node_count]));

  if (slot_arena == NULL || index_map_arena == NULL ||
      node_value_arena == NULL) {
    log_oom();
    abort();
  }

  /*
   * Nodes at the current level being combined into parents.
   */
  TreeSlot *unmerged_node[256];
  int unmerged_node_len = leaf_node_count;

  for (int byte = 0, unmerged_node_i = 0; byte < 256; ++byte) {
    bool zero_frequency = byte_frequency[byte] == 0;
    if (zero_frequency)
      continue;

    TreeSlot *leaf = arena_alloc(slot_arena, sizeof(TreeSlot));
    if (leaf == NULL) {
      log_oom();
      abort();
    }

    leaf->type = TREE_LEAF;
    leaf->leaf.index = byte;
    leaf->leaf.value = &byte_frequency[byte];
    unmerged_node[unmerged_node_i++] = leaf;
  }

  TreeSlot *root_node;

  while (unmerged_node_len > 1) {
    // Find the two nodes with the lowest frequencies.
    TreeSlot *nodes_to_merge[2] = {NULL, NULL};
    int nodes_to_merge_indices[2] = {-1, -1};

    for (int i = 0; i < unmerged_node_len; ++i) {
      TreeSlot *candidate = unmerged_node[i];
      uint64_t candidate_frequency = *(uint64_t *)tree_slot_value(candidate);

      uint64_t lowest_frequency =
          nodes_to_merge[0] != NULL
              ? *(uint64_t *)tree_slot_value(nodes_to_merge[0])
              : UINT64_MAX;

      uint64_t second_lowest_frequency =
          nodes_to_merge[1] != NULL
              ? *(uint64_t *)tree_slot_value(nodes_to_merge[1])
              : UINT64_MAX;

      if (candidate_frequency < lowest_frequency) {
        nodes_to_merge[1] = nodes_to_merge[0];
        nodes_to_merge_indices[1] = nodes_to_merge_indices[0];

        nodes_to_merge[0] = candidate;
        nodes_to_merge_indices[0] = i;
      } else if (candidate_frequency < second_lowest_frequency) {
        nodes_to_merge[1] = candidate;
        nodes_to_merge_indices[1] = i;
      }
    }

    // Build an index map for each child of the new parent.
    int *child_index_maps[2] = {NULL, NULL};
    int child_index_map_lengths[2] = {0, 0};

    for (int child = 0; child < 2; ++child) {
      TreeSlot *child_slot = nodes_to_merge[child];

      if (child_slot->type == TREE_LEAF) {
        child_index_map_lengths[child] = 1;

        child_index_maps[child] = arena_alloc(index_map_arena, sizeof(int));
        assert(child_index_maps[child] != NULL);

        child_index_maps[child][0] = child_slot->leaf.index;
      } else {
        int left_map_length = child_slot->node.index_map_len[0];
        int right_map_length = child_slot->node.index_map_len[1];

        child_index_map_lengths[child] = left_map_length + right_map_length;

        child_index_maps[child] = arena_alloc(
            index_map_arena, sizeof(int) * child_index_map_lengths[child]);
        assert(child_index_maps[child] != NULL);

        memcpy(child_index_maps[child], child_slot->node.index_map[0],
               sizeof(int) * left_map_length);

        memcpy(child_index_maps[child] + left_map_length,
               child_slot->node.index_map[1], sizeof(int) * right_map_length);
      }
    }

    // Create the parent node.
    TreeSlot *parent_slot = arena_alloc(slot_arena, sizeof(*parent_slot));
    assert(parent_slot != NULL);

    parent_slot->type = TREE_NODE;

    memcpy(parent_slot->node.slot, nodes_to_merge, sizeof(nodes_to_merge));

    memcpy(parent_slot->node.index_map, child_index_maps,
           sizeof(child_index_maps));

    memcpy(parent_slot->node.index_map_len, child_index_map_lengths,
           sizeof(child_index_map_lengths));

    uint64_t left_frequency = *(uint64_t *)tree_slot_value(nodes_to_merge[0]);
    uint64_t right_frequency = *(uint64_t *)tree_slot_value(nodes_to_merge[1]);

    uint64_t *parent_frequency =
        arena_alloc(node_value_arena, sizeof(*parent_frequency));
    assert(parent_frequency != NULL);

    *parent_frequency = left_frequency + right_frequency;
    parent_slot->node.value = parent_frequency;

    // Replace the first selected node with its parent.
    int first_index = nodes_to_merge_indices[0];
    int second_index = nodes_to_merge_indices[1];

    unmerged_node[first_index] = parent_slot;

    // Remove the second selected node, preserving the remaining order.
    memmove(&unmerged_node[second_index], &unmerged_node[second_index + 1],
            sizeof(*unmerged_node) * (unmerged_node_len - second_index - 1));

    --unmerged_node_len;

    if (unmerged_node_len == 1) {
      root_node = parent_slot;
    }
  }

  HuffmanCode *code = calloc(256, sizeof(HuffmanCode));

  for (uint32_t symbol = 0; symbol < UINT8_MAX + 1; ++symbol) {
    Path *path = find_path(symbol, root_node);
    if (path == NULL) {
      continue;
    }

    for (int i = 0; i < path->length; ++i) {
      code[symbol].code <<= 1;
      if (path->steps[i] == RIGHT) {
        code[symbol].code |= 0b1;
      }
    }
    code[symbol].bit_length = path->length;
    free_path(path);
  }

  arena_free(node_value_arena);
  arena_free(index_map_arena);
  arena_free(slot_arena);

  return code;
}
