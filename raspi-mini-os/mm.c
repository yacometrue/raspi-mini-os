#include "mm.h"
#include "atag.h"
#include "malloc.h"
#include <stddef.h>
#include <stdint.h>

/* def for page */
extern uint8_t __end;
static uint32_t num_pages;
static Header all_pages;
static Header free_pages;

void append_page(Header *free_pages, Page *page);
Page *pop_page(Header *free_pages);

void mm_init(atag_t *atags) {
    uint32_t mem_size, page_list_len, page_list_end, num_kernel_pages, i;

    mem_size = get_mem_size(atags);
    num_pages = mem_size / PAGE_SIZE;

    all_pages.size = num_pages;
    all_pages.head = (Page *)&__end;
    page_list_len = all_pages.size * sizeof(Page);
    my_memset(all_pages.head, 0, page_list_len);

    /*Initialize free pages*/
    free_pages.head = free_pages.tail = NULL;
    free_pages.size = 0;

    Page *cur;
    cur = all_pages.head;
    num_kernel_pages = ((uint32_t)&__end) / PAGE_SIZE;

    for (i = 0; i < num_kernel_pages; i++, cur++) {
        cur->address = i * PAGE_SIZE;
        cur->flags.kernel_page = 1;
        cur->flags.allocated = 1;
    }
    /* reserve 2MB for kernel*/
    for (; i < num_kernel_pages + (INIT_HEAP_SIZE / PAGE_SIZE); i++) {
        cur->address = i * PAGE_SIZE;
        cur->flags.kernel_page = 1;
        cur->flags.allocated = 1;
    }

    for (; i < num_pages; i++, cur++) {
        cur->flags.allocated = 0;
        append_page(&free_pages, cur);
    }
    all_pages.tail = free_pages.tail;
    page_list_end = (uint32_t)&__end + page_list_len; /* should equal to all_pages.tail++??*/
    heap_init((void*)page_list_end);
}

void append_page(Header *free_pages, Page *page) {
    if (free_pages->head == NULL) /*tail should also be NULL*/ 
        free_pages->head = page;
    else
        free_pages->tail->next = page; 
    page->prev = free_pages->tail;
    page->next = NULL;
    free_pages->tail = page;
    (free_pages->size)++;
}

Page *pop_page(Header *free_pages) {
    Page *page;
    page = free_pages->head;
    free_pages->head = page->next;
    page->flags.allocated = 1;
    page->next = NULL;
    free_pages->head->prev = NULL;
    (free_pages->size)--;
    return page;
}

void *alloc_page(void) {
    Page *page;
    void *ptr;
    if (free_pages.size == 0) return NULL;
    page = pop_page(&free_pages);
    ptr = (void *)((page - all_pages.head) * PAGE_SIZE);
    my_memset(ptr, 0, PAGE_SIZE);
    return ptr;
}

void free_page(void *ptr) {
    Page *page;
    page = all_pages.head + ((uint32_t)ptr / PAGE_SIZE);
    page->flags.allocated = 0;
    append_page(&free_pages, page);
    return;
}
