#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>

#define IS_BLOCK_FREE 0x1
#define IS_EXTENDED 0x2
#define IS_READABLE 0x4
#define IS_WRITABLE 0x8

typedef struct memory_block {
  char header;
} block;

typedef struct block_ptr {
  block *block;
} ptr;

ptr main_ptr;
ptr current_block;
int block_size;
int blocks_count;
int memory_size;

int init_memory(int _block_size, int _blocks_count) {
  if (_block_size < 1 || _blocks_count < 1) return -1;
  memory_size = _block_size * _blocks_count;
  blocks_count = _blocks_count;
  block_size = _block_size;

  main_ptr.block = malloc(memory_size);
  ptr max_address = main_ptr;
  max_address.block+=memory_size;
  for (ptr allocation_pointer = main_ptr; allocation_pointer.block < max_address.block; allocation_pointer.block += _block_size) {
    allocation_pointer.block->header = 0b00001101;
  }
  current_block = main_ptr;
  return memory_size;
}



int main() {

  init_memory(64, 10);

  return 0;
}
