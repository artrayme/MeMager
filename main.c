#include <malloc.h>

#define IS_BLOCK_FREE 0x1
#define IS_EXTENDED 0x2
#define IS_READABLE 0x4
#define IS_WRITABLE 0x8
#define DEFAULT_BLOCK_STATE IS_BLOCK_FREE + IS_READABLE + IS_WRITABLE

#define SUCCESS 0
#define ILLEGAL_MEMORY_INIT_ARGUMENTS -1
#define TRY_TO_ALLOCATE_LESS_THAN_ONE_BYTE -1
#define TRY_TO_ALLOCATE_MORE_BYTES_THAN_AVAILABLE -2
#define TRY_TO_FREE_ERROR_POINTER -1
#define NULL_POINTER 1

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

void copy_ptr(ptr *source, ptr *dist) {
  dist->block = source->block;
  dist->block->header = source->block->header;
  dist->error = source->error;
}

int init_memory(int _block_size, int _blocks_count) {
  if (_block_size < 1 || _blocks_count < 1) return ILLEGAL_MEMORY_INIT_ARGUMENTS;
  if (memory_size > 0) {
    free(main_ptr.block);
  }
  memory_size = _block_size * _blocks_count;
  blocks_count = _blocks_count;
  block_size = _block_size;

  main_ptr.block = malloc(memory_size);
  ptr max_address = main_ptr;
  max_address.block += memory_size;
  for (ptr allocation_pointer = main_ptr; allocation_pointer.block < max_address.block; allocation_pointer.block += _block_size) {
    allocation_pointer.block->header = DEFAULT_BLOCK_STATE;
    //    allocation_pointer.block->header |= IS_BLOCK_FREE;
    //    allocation_pointer.block->header |= !IS_EXTENDED;
    //    allocation_pointer.block->header |= IS_READABLE;
    //    allocation_pointer.block->header |= IS_WRITABLE;
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
    size_error.block = 0;
    size_error.error = TRY_TO_ALLOCATE_LESS_THAN_ONE_BYTE;
    return size_error;
  } else if (size <= calc_real_available_memory()) {
    int allocated_memory = 0;
    ptr started_value;
    copy_ptr(&current_block_ptr, &started_value);
    while (size > allocated_memory) {
      allocated_memory += block_size - sizeof(block);
      current_block_ptr.block->header |= IS_EXTENDED;
      current_block_ptr.block->header ^= IS_BLOCK_FREE;
      current_block_ptr.block += block_size;
    }
    (current_block_ptr.block - block_size)->header ^= IS_EXTENDED;
    return started_value;
  }
  ptr size_error;
  size_error.block = 0;
  size_error.error = TRY_TO_ALLOCATE_MORE_BYTES_THAN_AVAILABLE;
  return size_error;
}

void print_block_info(char *name, ptr block_pointer);

int free_ptr(ptr *pointer) {
  if (pointer->error != SUCCESS) {
    return TRY_TO_FREE_ERROR_POINTER;
  }

  int freed_blocks_count = 0;
  while (pointer->block->header & IS_EXTENDED) {
    pointer->block->header = DEFAULT_BLOCK_STATE;
    pointer->block += block_size;
  }
  pointer->block->header = DEFAULT_BLOCK_STATE;

  if (pointer->block + block_size == current_block_ptr.block) {
    current_block_ptr.block -= freed_blocks_count * block_size;
    while ((current_block_ptr.block - block_size)->header & IS_BLOCK_FREE && current_block_ptr.block != main_ptr.block) {
      current_block_ptr.block -= block_size;
    }
  }
  pointer->error = NULL_POINTER;

  return SUCCESS;
}

void print_block_info(char *name, ptr block_pointer) {
  printf("---------- %s ---------\n", name);

  printf("real_address = %ul\n", block_pointer.block);
  if (block_pointer.error != 0) {
    printf("error = %d\n", block_pointer.error);
  } else {
    printf("flags: \n");
    printf("\tis_free =     %d\n", (block_pointer.block->header & IS_BLOCK_FREE) > 0);
    printf("\tis_extended = %d\n", (block_pointer.block->header & IS_EXTENDED) > 0);
    printf("\tis_readable = %d\n", (block_pointer.block->header & IS_READABLE) > 0);
    printf("\tis_writable = %d\n", (block_pointer.block->header & IS_WRITABLE) > 0);

    if (block_pointer.block->header & IS_EXTENDED) {
      int nested_blocks_count = 0;
      ptr cur_block = block_pointer;
      while ((cur_block.block->header & IS_EXTENDED) && nested_blocks_count < blocks_count) {
        nested_blocks_count++;
        cur_block.block += block_size;
      }
      printf("nested_blocks_count = %d: \n", nested_blocks_count);

      printf("last_block_in_nested: \n");
      printf("\treal_address = %ul\n", cur_block.block);
      printf("\tflags: \n");
      printf("\t\tis_free =     %d\n", (cur_block.block->header & IS_BLOCK_FREE) > 0);
      printf("\t\tis_extended = %d\n", (cur_block.block->header & IS_EXTENDED) > 0);
      printf("\t\tis_readable = %d\n", (cur_block.block->header & IS_READABLE) > 0);
      printf("\t\tis_writable = %d\n", (cur_block.block->header & IS_WRITABLE) > 0);
    }
    printf("---------------------------------\n");
  }
}

int main() {

  init_memory(32, 10);
  print_block_info("current after init", current_block_ptr);
  ptr x = alloc(100);
  ptr y = alloc(100);
  ptr z = alloc(100);
  ptr a = alloc(100);
  ptr b = alloc(1);
  ptr c = alloc(1);
  ptr d = alloc(1);
  ptr e = alloc(1);
  print_block_info("x allocated pointer", x);
  print_block_info("y allocated pointer", y);
  print_block_info("z allocated pointer", z);
  print_block_info("a allocated pointer", a);
  print_block_info("b allocated pointer", b);
  print_block_info("c allocated pointer", c);
  print_block_info("d allocated pointer", d);
  print_block_info("e allocated pointer", e);
  print_block_info("current pointer", current_block_ptr);

  free_ptr(x);

  return 0;
}