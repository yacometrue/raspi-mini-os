#include "atag.h"
#include <stddef.h>
#include <stdint.h>
#ifndef MALLOC_H
#define MALLOC_H

#define __REG_TYPE      volatile uint32_t
#define INIT_HEAP_SIZE  ((__REG_TYPE) 512 * 1024) //reserved 0.5 MB = 2 ^ 19
#define MAX_HEAP_SIZE   ((__REG_TYPE) 2048 * 1024) //max 2 MB = 2 ^ 21
#define SEG_LISTS_NUM   ((__REG_TYPE) 10)
#define MAX_BLOCK_SIZE  ((__REG_TYPE) 8192)
#define MEM_PER_SEG     ((__REG_TYPE) (32768 + sizeof(heap_header)) //2^15 + header

typedef struct heap {
    struct heap* next;
} Heap;

typedef struct {
    Heap* head;
    Heap* tail;
    uint32_t block_size;
} heap_header;

void my_memset(void* dest, int val, size_t len);
void heap_init(void* start);
void* malloc(uint32_t ptr);
void free(void* ptr);

#endif
