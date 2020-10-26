#include "mm.h"
#include "atag.h"
#include <stddef.h>
#include <stdint.h>

/* def for page */
extern uint8_t __end;
static uint32_t num_pages;
static Header all_pages;
static Header free_pages;

/* def for heap */
static Heap head;
static Heap *free_ptr;

void my_memset(void *dest, int val, size_t len) {
    unsigned char *ptr = dest;
    while (len-- > 0) *ptr++ = val;
}

void heap_init(void *start) {
    my_memset(start, 0, sizeof(Heap));
    head = *(Heap *)start;
    head.seg_size = HEAP_SIZE;
    head.prev = NULL;
    head.next = NULL;
    free_ptr = &head;
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

void mvptr_bef_freeptr(Heap *ptr) {
    if (ptr->prev) ptr->prev->next = ptr->next; // maybe have more elegant way to avoid null pointer reference
    if (ptr->next) ptr->next->prev = ptr->prev;
    if (free_ptr->prev) free_ptr->prev->next = ptr;
    ptr->prev = free_ptr->prev;
    ptr->next = free_ptr;
}

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
    for (; i < num_kernel_pages + (HEAP_SIZE / PAGE_SIZE); i++) {
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
/*best fit policy*/
void *my_malloc(uint32_t nbytes) {
    Heap *ptr, *best_fit;
    int32_t tmp_diff, min_diff = 0x7FFFFFFF;
    
    nbytes += sizeof(Heap);
    /*4 bytes alignment*/
    nbytes += (nbytes & 0x3) ? (0x4 - (nbytes & 0x3)) : 0;

    if (free_ptr == NULL) return NULL;
    /* traverse all seg find minimum difference */
    for (ptr = free_ptr; ptr ;ptr = ptr->next) {
        if (ptr->seg_size >= nbytes) {
            tmp_diff = ptr->seg_size - nbytes;
            if (min_diff > tmp_diff) {
                min_diff = tmp_diff;
                best_fit = ptr;
                if (tmp_diff == 0) {
                    mvptr_bef_freeptr(ptr);
                    return best_fit++;
                }
            }
        }
    }

    if (min_diff > (int)(2 * sizeof(Heap))) {
        my_memset((void *)(best_fit + nbytes), 0, sizeof(Heap));
        ptr = ptr->next;
        best_fit->next = best_fit + nbytes;
        best_fit->next->prev = best_fit;
        best_fit->next->next = ptr;
        best_fit->next->seg_size = best_fit->seg_size - nbytes;
        best_fit->seg_size = nbytes;
    }

    ptr = best_fit;
    if (ptr->prev) ptr->prev->next = ptr->next; // maybe have more elegant way to avoid null pointer reference
    if (ptr->next) ptr->next->prev = ptr->prev;
    if (free_ptr->prev) free_ptr->prev->next = ptr;
    ptr->prev = free_ptr->prev;
    ptr->next = free_ptr;
   
    return best_fit++;
}

void my_free(void *ptr) {
    Heap *seg;
    if (!ptr) return;
    seg = ptr - sizeof(Heap);
    mvptr_bef_freeptr(seg);
    free_ptr = seg;
}
