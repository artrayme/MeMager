//
// Created by artrayme on 11/17/21.
//

#include "memager.h"
#include "memager_tester.h"
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
      if (i < 1 || j < 1) {
        if (res == ILLEGAL_MEMORY_INIT_ARGUMENTS) {
          printf("PASSED test number %d\n", test_number);
        } else {
          printf("FAILED test number %d: i = %d, j = %d, res = %d\n", test_number, i, j, res);
        }
      } else {
        if (res == i * j) {
          printf("PASSED test number %d\n", test_number);
        } else {
          printf("FAILED test number %d: i = %d, j = %d, res = %d\n", test_number, i, j, res);
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
//  init_memory(64, 10);
//  ptr ptr1 = alloc_ptr(1);
//  free_ptr(&ptr1);
//  test_one_free(1, ptr1, NULL_POINTER, current_block_ptr.block == main_ptr.block);
//
//  ptr ptr2 = alloc_ptr(64);
//  free_ptr(&ptr2);
//  test_one_free(2, ptr2, NULL_POINTER, current_block_ptr.block == main_ptr.block);
//
//  ptr ptr3 = alloc_ptr(300);
//  free_ptr(&ptr3);
//  test_one_free(3, ptr3, NULL_POINTER, current_block_ptr.block == main_ptr.block);
//
//  ptr ptr11 = alloc_ptr(1);
//  ptr ptr12 = alloc_ptr(1);
//  ptr ptr13 = alloc_ptr(1);
//  free_ptr(&ptr11);
//  test_one_free(4, ptr11, NULL_POINTER, current_block_ptr.block != main_ptr.block);

//  free_ptr(&ptr12);
//  test_one_free(5, ptr11, NULL_POINTER, current_block_ptr.block != main_ptr.block);

//  ptr back_ptr13;
//  copy_ptr(&ptr13, &back_ptr13);
//  free_ptr(&ptr13);
//  test_one_free(6, ptr13, NULL_POINTER, current_block_ptr.block == back_ptr13.block);
}