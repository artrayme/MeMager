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
#define TRY_TO_WRITE_MORE_BYTES_THEN_AVAILABLE -1

typedef struct memory_block {
  unsigned char header;
} block_header;

typedef struct block_ptr {
  block_header *block;
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
    //    allocation_pointer.block_header->header |= IS_BLOCK_FREE;
    //    allocation_pointer.block_header->header |= !IS_EXTENDED;
    //    allocation_pointer.block_header->header |= IS_READABLE;
    //    allocation_pointer.block_header->header |= IS_WRITABLE;
  }
  current_block_ptr = main_ptr;
  return memory_size;
}

int get_extended_blocks_count(ptr pointer) {
  return pointer.block->header >> 4;
}
void set_extended_blocks_count(ptr pointer, int count) {
  char result_header = pointer.block->header & 0b00001111;
  result_header |= (count << 4);
  pointer.block->header = result_header;
}

int calc_real_available_memory() {
  return main_ptr.block + memory_size - current_block_ptr.block - (main_ptr.block + memory_size - current_block_ptr.block) / block_size * sizeof(block_header);
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
    int extended_blocks_count = 0;
    allocated_memory += block_size - sizeof(block_header);
    current_block_ptr.block += block_size;
    while (size > allocated_memory) {
      allocated_memory += block_size - sizeof(block_header);
      current_block_ptr.block->header = 0;
      current_block_ptr.block += block_size;
      extended_blocks_count++;
    }
    started_value.block->header ^= IS_BLOCK_FREE;
    if (extended_blocks_count > 0) {
      set_extended_blocks_count(started_value, extended_blocks_count);
      started_value.block->header |= IS_EXTENDED;
    }
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

  if ((pointer->block + ((pointer->block->header >> 4) + 1) * block_size) == current_block_ptr.block) {
    current_block_ptr.block = pointer->block;
    set_extended_blocks_count(current_block_ptr, 0);
    current_block_ptr.block->header ^= !IS_EXTENDED;
    current_block_ptr.block->header ^= IS_BLOCK_FREE;
  } else {
    pointer->block->header ^= IS_BLOCK_FREE;
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
    printf("\textended_count = %d\n", (block_pointer.block->header >> 4));
    printf("\tis_free =        %d\n", (block_pointer.block->header & IS_BLOCK_FREE) > 0);
    printf("\tis_extended =    %d\n", (block_pointer.block->header & IS_EXTENDED) > 0);
    printf("\tis_readable =    %d\n", (block_pointer.block->header & IS_READABLE) > 0);
    printf("\tis_writable =    %d\n", (block_pointer.block->header & IS_WRITABLE) > 0);
    printf("---------------------------------\n");
  }
}

void test_memory_init() {
  for (int i = -1; i < 100; ++i) {
    for (int j = -1; j < 100; ++j) {
      int res = init_memory(i, j);
      if (i < 1 || j < 1) {
        if (res == ILLEGAL_MEMORY_INIT_ARGUMENTS) {
          printf("PASSED\n");
        } else {
          printf("FAILED: i = %d, j = %d, res = %d\n", i, j, res);
        }
      } else {
        if (res == i * j) {
          printf("PASSED\n");
        } else {
          printf("FAILED: i = %d, j = %d, res = %d\n", i, j, res);
        }
      }
    }
  }
}

void test_one_alloc(int test_number, ptr *pointer, int expected_error, char expected_flags) {
  if (expected_error == SUCCESS) {
    if (pointer->block->header == expected_flags) {
      printf("PASSED alloc test number %d\n", test_number);
    } else {
      printf("FAILED alloc test number %d\n", test_number);
      print_block_info("", *pointer);
    }
  } else {
    if (pointer->error == expected_error) {
      printf("PASSED alloc test number %d\n", test_number);
    } else {
      printf("FAILED alloc test number %d\n", test_number);
      print_block_info("", *pointer);
    }
  }
}

void test_memory_alloc() {
  init_memory(64, 10);
  ptr ptr1 = alloc(1);
  test_one_alloc(1, &ptr1, 0, IS_READABLE + IS_WRITABLE);

  ptr ptr2 = alloc(63);
  test_one_alloc(2, &ptr2, 0, IS_READABLE + IS_WRITABLE);

  ptr ptr3 = alloc(64);
  test_one_alloc(3, &ptr3, 0, 0b00010000 + IS_READABLE + IS_WRITABLE + IS_EXTENDED);

  ptr ptr4 = alloc(-1);
  test_one_alloc(4, &ptr4, TRY_TO_ALLOCATE_LESS_THAN_ONE_BYTE, -1);

  ptr ptr5 = alloc(10 * 64);
  test_one_alloc(5, &ptr5, TRY_TO_ALLOCATE_MORE_BYTES_THAN_AVAILABLE, -1);
}

void test_one_free(int test_number, ptr pointer, int expected_error, int assert_true) {
  if (pointer.error == NULL_POINTER && assert_true) {
    printf("PASSED free test number %d\n", test_number);
  } else {
    printf("FAILED free test number %d\n", test_number);
  }
}

void test_memory_free() {
  init_memory(64, 10);
  ptr ptr1 = alloc(1);
  free_ptr(&ptr1);
  test_one_free(1, ptr1, NULL_POINTER, current_block_ptr.block == main_ptr.block);

  ptr ptr2 = alloc(64);
  free_ptr(&ptr2);
  test_one_free(2, ptr2, NULL_POINTER, current_block_ptr.block == main_ptr.block);

  ptr ptr3 = alloc(300);
  free_ptr(&ptr3);
  test_one_free(3, ptr3, NULL_POINTER, current_block_ptr.block == main_ptr.block);

  ptr ptr11 = alloc(1);
  ptr ptr12 = alloc(1);
  ptr ptr13 = alloc(1);
  free_ptr(&ptr11);
  test_one_free(4, ptr11, NULL_POINTER, current_block_ptr.block != main_ptr.block);

  free_ptr(&ptr12);
  test_one_free(5, ptr11, NULL_POINTER, current_block_ptr.block != main_ptr.block);

  ptr back_ptr13;
  copy_ptr(&ptr13, &back_ptr13);
  free_ptr(&ptr13);
  test_one_free(6, ptr13, NULL_POINTER, current_block_ptr.block == back_ptr13.block);
}

int main() {
  //  test_memory_init();
  test_memory_alloc();
  test_memory_free();
  return 0;
}