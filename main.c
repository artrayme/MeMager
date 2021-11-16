#include <malloc.h>

#define IS_BLOCK_FREE 0x1
#define IS_EXTENDED 0x2
#define IS_READABLE 0x4
#define IS_WRITABLE 0x8

typedef struct memory_block {
  char header;
} block;

typedef struct block_ptr {
  block *block;
  int error;
} ptr;

ptr main_ptr;
ptr current_block_ptr;
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
  max_address.block += memory_size;
  for (ptr allocation_pointer = main_ptr; allocation_pointer.block < max_address.block; allocation_pointer.block += _block_size) {
    allocation_pointer.block->header = 0;
    allocation_pointer.block->header |= IS_BLOCK_FREE;
    allocation_pointer.block->header |= !IS_EXTENDED;
    allocation_pointer.block->header |= IS_READABLE;
    allocation_pointer.block->header |= IS_WRITABLE;
  }
  current_block_ptr = main_ptr;
  return memory_size;
}

int calc_real_available_memory() {
  return main_ptr.block + memory_size - current_block_ptr.block - (main_ptr.block + memory_size - current_block_ptr.block) / block_size * sizeof(block);
}

ptr alloc(int size) {
  if (size < 1) {
    ptr size_error;
    size_error.error = 1;
    return size_error;
  } else if (size <= calc_real_available_memory()) {
    int allocated_memory = 0;
    ptr started_value = current_block_ptr;
    while (size > allocated_memory) {
      allocated_memory += block_size - sizeof(block);
      current_block_ptr.block->header |= IS_EXTENDED;
      current_block_ptr.block += block_size;
    }
    current_block_ptr.block->header |= !IS_EXTENDED;
    return started_value;
  }
  ptr size_error;
  size_error.error = 1;
  return size_error;
}

int main() {

  init_memory(64, 10);
  ptr x = alloc(631);

  return 0;
}
