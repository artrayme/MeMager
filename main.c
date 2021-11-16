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

int main() {

//  init_memory(64, 10);

  return 0;
}
