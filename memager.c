//
// Created by artrayme on 11/17/21.
//

#include "memager.h"
#include <malloc.h>

ptr main_ptr;
ptr current_block_ptr;
int block_size;
int blocks_count;
int memory_size;

void copy_ptr(ptr *source, ptr *dest) {
  dest->block = source->block;
  dest->error = source->error;
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
  }
  current_block_ptr = main_ptr;
  return memory_size;
}

unsigned char get_extended_blocks_count(ptr pointer) {
  return pointer.block->header >> 4;
}
void set_extended_blocks_count(ptr pointer, int count) {
  char result_header = pointer.block->header & 0b00001111;
  result_header |= (count << 4);
  pointer.block->header = result_header;
}

int calc_real_available_memory() {
  return main_ptr.block + memory_size - current_block_ptr.block - (main_ptr.block + memory_size - current_block_ptr.block) / block_size * sizeof(block_struct);
}

ptr alloc_ptr(int size) {
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
    allocated_memory += block_size - sizeof(block_struct);
    current_block_ptr.block += block_size;
    while (size > allocated_memory) {
      allocated_memory += block_size - sizeof(block_struct);
      current_block_ptr.block->header = 0;
      current_block_ptr.block += block_size;
      extended_blocks_count++;
    }
    started_value.block->header = DEFAULT_BLOCK_STATE;
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

int free_ptr(ptr *pointer) {
  if (pointer->error != SUCCESS) {
    return TRY_TO_FREE_ERROR_POINTER;
  }

//  check is this block last
  if ((pointer->block + ((pointer->block->header >> 4) + 1) * block_size) == current_block_ptr.block) {
    current_block_ptr.block = pointer->block;
    set_extended_blocks_count(current_block_ptr, 0);
    current_block_ptr.block->header ^= !IS_EXTENDED;
    current_block_ptr.block->header ^= IS_BLOCK_FREE;
    ptr temp;
    copy_ptr(&main_ptr, &temp);
    int blocks_count_to_deleting = 0;
    while (temp.block!=current_block_ptr.block){
      unsigned char ext_blocks = get_extended_blocks_count(temp)+1;
      if (temp.block->header&IS_BLOCK_FREE){
        blocks_count_to_deleting+= ext_blocks;
      } else{
        blocks_count_to_deleting=0;
      }
      temp.block+= ext_blocks*block_size;
    }
    current_block_ptr.block-=blocks_count_to_deleting*block_size;
  } else {
    pointer->block->header ^= IS_BLOCK_FREE;
  }
  pointer->error = NULL_POINTER;

  return SUCCESS;
}

int read(ptr pointer, void *buffer, int size) {
  if (pointer.error != SUCCESS) {
    return TRY_TO_READ_NULL_POINTER;
  }
  if (get_extended_blocks_count(pointer) > size) {
    return TRY_TO_READ_MORE_BYTES_THEN_AVAILABLE;
  }
  char *void_ptr = (char *) pointer.block;
  char *buf = (char *) buffer;
  for (int i = 0; i < size; ++i) {
    buf[i] = void_ptr[1 + i];
  }
  return SUCCESS;
}

int write(ptr pointer, void *buffer, int size) {
  if (pointer.error != SUCCESS) {
    return TRY_TO_WRITE_NULL_POINTER;
  }
  if ((get_extended_blocks_count(pointer)+1)*block_size < size) {
    return TRY_TO_WRITE_MORE_BYTES_THEN_AVAILABLE;
  }
  char *void_ptr = (char *) pointer.block;
  char *buf = (char *) buffer;
  for (int i = 0; i < size; ++i) {
    void_ptr[1 + i] = buf[i];
  }
  return SUCCESS;
}

int move_ptr(ptr *source_ptr, ptr *dest_ptr){
  if (source_ptr->error!=SUCCESS){
    return TRY_TO_REWRITE_ERROR_POINTER;
  }
  if (get_extended_blocks_count(*source_ptr) > get_extended_blocks_count(*dest_ptr)){
    return TRY_TO_REWRITE_BIGGER_POINTER_THEN_SOURCE;
  }

  int blocks_to_copy_count = get_extended_blocks_count(*source_ptr)+1;
  for (int i = 1; i < blocks_to_copy_count; ++i) {
    dest_ptr->block[i] = source_ptr->block[i];
  }
  free_ptr(source_ptr);

  return SUCCESS;
}


ptr get_memory_start_ptr() {
  return main_ptr;
}

ptr get_memory_current_ptr() {
  return current_block_ptr;
}
