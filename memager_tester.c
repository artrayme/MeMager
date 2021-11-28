//
// Created by artrayme on 11/17/21.
//

#include "memager_tester.h"
#include "memager.h"
#include <stdio.h>

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
  int test_number = 0;
  for (int i = -1; i < 10; ++i) {
    for (int j = -1; j < 10; ++j) {
      test_number++;
      int res = init_memory(i, j);
      if (i < 0 || j < 0) {
        if (res == ILLEGAL_MEMORY_INIT_ARGUMENTS) {
          printf("PASSED init test number %d\n", test_number);
        } else {
          printf("FAILED init test number %d: i = %d, j = %d, res = %d\n", test_number, i, j, res);
        }
      } else {
        if (res == i * j) {
          printf("PASSED init test number %d\n", test_number);
        } else {
          printf("FAILED init test number %d: i = %d, j = %d, res = %d\n", test_number, i, j, res);
        }
      }
    }
  }
}

void test_one_alloc(int test_number, ptr *pointer, int expected_error, char expected_flags) {
  if (expected_error == SUCCESS) {
    if (pointer->block->header == expected_flags) {
      printf("PASSED alloc_ptr test number %d\n", test_number);
    } else {
      printf("FAILED alloc_ptr test number %d\n", test_number);
      print_block_info("", *pointer);
    }
  } else {
    if (pointer->error == expected_error) {
      printf("PASSED alloc_ptr test number %d\n", test_number);
    } else {
      printf("FAILED alloc_ptr test number %d\n", test_number);
      print_block_info("", *pointer);
    }
  }
}

void test_memory_alloc() {
  init_memory(64, 10);
  ptr ptr1 = alloc_ptr(1);
  test_one_alloc(1, &ptr1, 0, IS_READABLE + IS_WRITABLE);

  ptr ptr2 = alloc_ptr(48);
  test_one_alloc(2, &ptr2, 0, IS_READABLE + IS_WRITABLE);

  ptr ptr3 = alloc_ptr(64);
  test_one_alloc(3, &ptr3, 0, 0b00010000 + IS_READABLE + IS_WRITABLE + IS_EXTENDED);

  ptr ptr4 = alloc_ptr(-1);
  test_one_alloc(4, &ptr4, TRY_TO_ALLOCATE_LESS_THAN_ONE_BYTE, -1);

  ptr ptr5 = alloc_ptr(10 * 64);
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
  ptr ptr1 = alloc_ptr(1);
  free_ptr(&ptr1);
  test_one_free(1, ptr1, NULL_POINTER, get_memory_start_ptr().block == get_memory_current_ptr().block);

  ptr ptr2 = alloc_ptr(64);
  free_ptr(&ptr2);
  test_one_free(2, ptr2, NULL_POINTER, get_memory_current_ptr().block == get_memory_start_ptr().block);

  ptr ptr3 = alloc_ptr(300);
  free_ptr(&ptr3);
  test_one_free(3, ptr3, NULL_POINTER, get_memory_current_ptr().block == get_memory_start_ptr().block);

  ptr ptr11 = alloc_ptr(1);
  ptr ptr12 = alloc_ptr(1);
  ptr ptr13 = alloc_ptr(1);
  free_ptr(&ptr11);
  test_one_free(4, ptr11, NULL_POINTER, get_memory_current_ptr().block != get_memory_start_ptr().block);

  free_ptr(&ptr12);
  test_one_free(5, ptr11, NULL_POINTER, get_memory_current_ptr().block != get_memory_start_ptr().block);

  free_ptr(&ptr13);
  test_one_free(6, ptr13, NULL_POINTER, get_memory_current_ptr().block == get_memory_start_ptr().block);
}

void test_memory_read_and_write() {
  init_memory(10, 10);
  ptr ptr1 = alloc_ptr(1);
  int to_write_value = 123;
  write(ptr1, &to_write_value, sizeof(to_write_value));
  int result_value = 0;
  read(ptr1, &result_value, sizeof(result_value));
  if (to_write_value == result_value) {
    printf("PASSED read/write test number %d\n", 1);
  } else {
    printf("FAILED read/write test number %d\n", 1);
  }

  typedef struct test {
    long a;
    size_t b;
    char c;
  } test_struct;
  ptr ptr2 = alloc_ptr(sizeof(test_struct));
  test_struct to_write_struct = {.a = 123, .b = 456, .c = 9};
  write(ptr2, &to_write_struct, sizeof(to_write_struct));
  test_struct read_struct;
  read(ptr2, &read_struct, sizeof(read_struct));
  if (read_struct.a == to_write_struct.a && read_struct.b == to_write_struct.b && read_struct.c == to_write_struct.c) {
    printf("PASSED read/write test number %d\n", 2);
  } else {
    printf("FAILED read/write test number %d\n", 2);
    printf("a = %ld, b = %zu, c = %d\n", read_struct.a, read_struct.b, read_struct.c);
  }
}

void test_memory_move(){
  init_memory(64, 10);
  ptr ptr1 = alloc_ptr(65);
  ptr ptr2 = alloc_ptr(10);
  int res = move_ptr(&ptr2, &ptr1);
  if (res == SUCCESS && ptr1.block == get_memory_start_ptr().block && (ptr1.block+2*64) == get_memory_current_ptr().block) {
    printf("PASSED move test number %d\n", 1);
  } else {
    printf("FAILED move test number %d\n", 1);
  }

  init_memory(64, 10);
  ptr ptr3 = alloc_ptr(65);
  ptr ptr4 = alloc_ptr(10);
  int res2 = move_ptr(&ptr3, &ptr4);
  if (res2 == TRY_TO_REWRITE_BIGGER_POINTER_THEN_SOURCE) {
    printf("PASSED move test number %d\n", 2);
  } else {
    printf("FAILED move test number %d\n", 2);
  }

}
