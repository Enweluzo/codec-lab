#ifndef AV_HUFFMAN_H
#define AV_HUFFMAN_H
#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint64_t code;
  uint8_t bit_length;
} HuffmanCode;

/*
 * Returns a Huffman code for every possible 8-bit symbol (0x00 – 0xFF).
 * The returned array is indexed by the symbol value, so huffcode[0xFF]
 * contains the Huffman code for the 0xFF symbol.
 */
extern HuffmanCode *get_huffman_code(const void *_src, size_t size);

#endif
