//
// Created by artrayme on 11/17/21.
//

#ifndef MEMAGER__MEMAGER_H_
#define MEMAGER__MEMAGER_H_

#endif//MEMAGER__MEMAGER_H_

//Block header masks
#define IS_BLOCK_FREE 0x1
#define IS_EXTENDED 0x2
#define IS_READABLE 0x4
#define IS_WRITABLE 0x8
#define EXTENDED_SIZE 0xF0
#define DEFAULT_BLOCK_STATE IS_BLOCK_FREE + IS_READABLE + IS_WRITABLE

#define SUCCESS 0
#define NULL_POINTER 1

#define ILLEGAL_MEMORY_INIT_ARGUMENTS -1

#define TRY_TO_ALLOCATE_LESS_THAN_ONE_BYTE -1
#define TRY_TO_ALLOCATE_MORE_BYTES_THAN_AVAILABLE -2

#define TRY_TO_FREE_ERROR_POINTER -1

#define TRY_TO_WRITE_MORE_BYTES_THEN_AVAILABLE -1
#define TRY_TO_WRITE_NULL_POINTER -2

#define TRY_TO_READ_MORE_BYTES_THEN_AVAILABLE -1
#define TRY_TO_READ_NULL_POINTER -2

#define TRY_TO_REWRITE_ERROR_POINTER -1
#define TRY_TO_REWRITE_BIGGER_POINTER_THEN_SOURCE -2

// Block structure
typedef struct memory_block {
  unsigned char header;
} block_struct;

// Block pointer
typedef struct block_ptr {
  block_struct *block;
  int error;
} ptr;

/*
 * Memory initialisation function.
 * Memory is the singleton. Then, if you call this function,
 * old pointers become corrupted.
 *
 * This function makes a system call to take memory from the system
 * to be worked with. This function also clears the occupied memory from the last initialisation
 *
 * You can free all memory occupied by this manager by passing the arguments (0,0)
 *
 * @param _block_size - one block size in bytes. Should be a positive value greater than 1.
 * @param _block_count - initialised blocks count. Should be a positive value greater than 1.
 * @return - status (SUCCESS, ILLEGAL_MEMORY_INIT_ARGUMENTS)
 * */
int init_memory(int _block_size, int _blocks_count);

/*
 * Pointer allocation function.
 *
 * You must pass size of needed memory, but will be allocated memory more,
 * then 'size' argument, because memory allocated by blocks. Also, the block size will
 * include the header size. For example, if you were inited memory with block size =64
 * and then try to allocate pointer with size 64 - actually, will be allocated 128 bytes.
 * This is because 64 bytes of needed memory plus the byte (or bytes) for header are greater than
 * the size of one block (64). So, in this situation the 128 bytes are allocated.
 *
 * @param size - size of needed memory.
 * @return pointer - if memory successfully allocated, pointer error attribute will be SUCCESS.
 * Otherwise, error field will be TRY_TO_ALLOCATE_LESS_THAN_ONE_BYTE or TRY_TO_ALLOCATE_MORE_BYTES_THAN_AVAILABLE codes.
 * */
ptr alloc_ptr(int size);

/*
 * Pointer freeing function.
 *
 * After calling this function, pointer error filed will be NULL_POINTER.
 *
 * @param pointer - a pointer to the memory you want to free
 * @return error code - SUCCESS, TRY_TO_ALLOCATE_LESS_THAN_ONE_BYTE, TRY_TO_ALLOCATE_MORE_BYTES_THAN_AVAILABLE
 * */
int free_ptr(ptr *pointer);

/*
 * Data write function.
 *
 * Byte-by-byte copy from buffer to memory block.
 *
 * @param pointer - a pointer to the memory where you want to write the data.
 * @param buffer - data
 * @param size - num of bytes to write
 * @param - error code - SUCCESS, TRY_TO_WRITE_MORE_BYTES_THEN_AVAILABLE, TRY_TO_WRITE_NULL_POINTER
 * */
int write(ptr pointer, void *buffer, int size);

/*
 * Data read function.
 *
 * Byte-by-byte copy from memory block to buffer.
 *
 * @param pointer - a pointer to the memory where you want to read the data.
 * @param buffer - data
 * @param size - num of bytes to read
 * @return - error code. SUCCESS, TRY_TO_READ_MORE_BYTES_THEN_AVAILABLE, TRY_TO_READ_NULL_POINTER
 * */
int read(ptr pointer, void *buffer, int size);

/*
 * Utility function for correct pointers coping.
 *
 * @param source - source pointer.
 * @param dest - destination pointer.
 * */
void copy_ptr(ptr *source, ptr *dest);

/*
 * Data move function.
 *
 * Copy all bytes from source pointer to dest pointer
 *
 * @param source_ptr - source pointer
 * @param dest_ptr - destination pointer
 * @return - error code: SUCCESS, TRY_TO_REWRITE_ERROR_POINTER, TRY_TO_REWRITE_BIGGER_POINTER_THEN_SOURCE
 * */
int move_ptr(ptr *source_ptr, ptr *dest_ptr);

// Only for testing
ptr get_memory_start_ptr();

// Only for testing
ptr get_memory_current_ptr();
