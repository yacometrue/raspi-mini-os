#include <stdint.h>

#include "atag.h"
#ifndef MM_H
#define MM_H

#define PAGE_SIZE 4096

typedef struct {
    uint8_t allocated : 1;
    uint8_t kernel_page : 1;
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

void mm_init(atag_t *atags);
void *alloc_page(void);
void free_page(void *ptr);

#endif
