#include <stdint.h>
#include "atag.h"
#ifndef MM_H
#define MM_H

#define PAGE_SIZE 4096
#define HEAP_SIZE 2048 * 1023 //reserved 2MB

typedef struct {
    uint8_t allocated : 1;
    uint8_t kernel_page : 1;
    uint8_t heap_pag: 1;
    uint32_t reserved : 30;
} page_flags;

typedef struct page {
    uint32_t address;
    struct page *next;
    struct page *prev;
    page_flags flags;
} Page;

typedef struct {
    Page *head;
    Page *tail;
    uint32_t size;
} Header;

typedef struct heap_seg {
    struct heap_seg *prev;
    struct heap_seg *next;
    uint32_t seg_size;
} Heap;

void mm_init(atag_t *atags);
void *alloc_page(void);
void free_page(void *ptr);

#endif
