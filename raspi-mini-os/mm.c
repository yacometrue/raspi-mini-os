#include "mm.h"

#include <stddef.h>
#include <stdint.h>

#include "atag.h"

extern uint8_t __end;

static uint32_t num_pages;
static Header all_pages;
static Header free_pages;

void memset(void *dest, int val, size_t len) {
    unsigned char *ptr = dest;
    while (len-- > 0) *ptr++ = val;
}

void mem_init(atag_t *atags) {
    uint32_t mem_size, page_list_len, num_kernel_pages, i;

    mem_size = get_mem_size(atags);
    num_pages = mem_size / PAGE_SIZE;

    all_pages.size = sizeof(Page) * num_pages;
    all_pages.head = (Page *)&__end;
    memset(all_pages.head, 0, page_list_len);

    Page *tmp;
    tmp = all_pages.head;
    num_kernel_pages = ((uint32_t)&__end) / PAGE_SIZE;
    for (i = 0; i < num_kernel_pages; i++) {
        tmp->address = i * PAGE_SIZE;
        tmp->flags.kernel_page = 1;
        tmp->flags.allocated = 1;
        tmp++;
    }

    free_pages.head = tmp;

    for (; i < num_pages; i++) {
        tmp->flags.allocated = 0;
        tmp++;
    }
    tmp--;
    free_pages.tail = all_pages.tail = tmp;
}

void *alloc_page(void) {
    Page *page;
    void *ptr;
    if (free_pages.size == 0) return NULL;

    page = free_pages.head;
    page->flags.allocated = 1;
    free_pages.head = page->next;
    page->next = NULL;
    free_pages.head->prev = NULL;
    free_pages.size--;

    ptr = (void *)((page - all_pages.head) * PAGE_SIZE);
    memset(ptr, 0, PAGE_SIZE);
    return ptr;
}

void free_page(void *ptr) {
    Page *page;
    Page *old_tail;
    page =  all_pages.head + ((uint32_t)ptr / PAGE_SIZE);
    page->flags.allocated = 0;
    old_tail = free_pages.tail;
    page->prev = old_tail;
    page->next = NULL;
    old_tail->next = page;
    free_pages.tail = page;
    return;
}
