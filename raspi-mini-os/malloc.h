#include <stddef.h>
#include <stdint.h>
#include "atag.h"
#ifndef MALLOC_H
#define MALLOC_H

#define INIT_HEAP_SIZE 512 * 1024 //reserved 0.5 MB
#define MAX_HEAP_SIZE 2048 * 1024 //max 2 MB
#define SEG_LISTS_NUM 10
#define MAX_BLOCK_SIZE 8192
#define MEM_PER_SEG   32768

typedef struct heap {
    struct heap *next;
    uint32_t allocated;
} heap;  

typedef struct {
    heap *head;
    uint32_t block_size;
} heap_header;


void my_memset(void *dest, int val, size_t len);
void heap_init(void *start);
void *malloc(uint32_t ptr);
void free(void *ptr);

#endif
