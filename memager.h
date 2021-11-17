//
// Created by artrayme on 11/17/21.
//

#ifndef MEMAGER__MEMAGER_H_
#define MEMAGER__MEMAGER_H_

#endif//MEMAGER__MEMAGER_H_

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
#define TRY_TO_WRITE_NULL_POINTER -2
#define TRY_TO_READ_MORE_BYTES_THEN_AVAILABLE -1
#define TRY_TO_READ_NULL_POINTER -2

typedef struct memory_block {
  unsigned char header;
} block_struct;

typedef struct block_ptr {
  block_struct *block;
  int error;
} ptr;

int init_memory(int _block_size, int _blocks_count);

ptr alloc_ptr(int size);

int free_ptr(ptr *pointer);

int read(ptr pointer, void *buffer, int size);

int write(ptr pointer, void *buffer, int size);

void copy_ptr(ptr *source, ptr *dist);

ptr get_memory_start_ptr();

ptr get_memory_current_ptr();
