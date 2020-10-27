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
//static Heap *free_ptr;

void my_memset(void *dest, int val, size_t len) {
    unsigned char *ptr = dest;
    while (len-- > 0) *ptr++ = val;
}

void heap_init(void *start) {
    my_memset(start, 0, sizeof(Heap));
    head = *(Heap *)start;
    head.seg_size = HEAP_SIZE;
    head.allocated = 0;
    head.prev = head.next = NULL;
 //   free_ptr = &head;
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
    Heap *ptr, *best_fit = NULL;
    int32_t tmp_diff, min_diff = 0x7FFFFFFF;
    
    nbytes += sizeof(Heap);
    /*Padding*/
    nbytes += (nbytes & 0x3) ? (0x4 - (nbytes & 0x3)) : 0;

    /* traverse all seg find minimum difference */
    for (ptr = &head; ptr ;ptr = ptr->next) {
        if (!ptr->allocated && ptr->seg_size >= nbytes) {
            tmp_diff = ptr->seg_size - nbytes;
            if (min_diff > tmp_diff) {
                min_diff = tmp_diff;
                best_fit = ptr;
                if (tmp_diff == 0) {
                    best_fit->allocated = 1;
                    return best_fit++; /* skip header */
                }
            }
        }
    }

    if (best_fit == NULL) return NULL;

    if (min_diff > (int)(2 * sizeof(Heap))) {
        my_memset((void *)(best_fit) + nbytes, 0, sizeof(Heap));
        ptr = best_fit->next;
        best_fit->next = ((void *)best_fit) + nbytes; // TODO: Be careful about type casting
        best_fit->next->prev = best_fit;
        best_fit->next->next = ptr;
        best_fit->next->seg_size = best_fit->seg_size - nbytes;
        best_fit->seg_size = nbytes;
    }

    best_fit->allocated = 1;

    return best_fit++; /* skip header */
}

void my_free(void *ptr) {
    if (!ptr) 
        return;
    Heap *seg;
    seg = ptr - sizeof(Heap);
    seg->allocated = 0;
    /* merge prev&next segment if not allocated*/
    if (seg->next && !seg->next->allocated) {
        seg->seg_size += sizeof(Heap) + seg->next->seg_size; 
        seg->next = seg->next->next;
        if (seg->next->next) seg->next->next->prev = seg;
    }
    if (seg->prev && !seg->prev->allocated) {
        seg = seg->prev;
        seg->seg_size += sizeof(Heap) + seg->next->seg_size; 
        if (seg->next->next) seg->next->next->prev = seg;
    }
}
