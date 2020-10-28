#include "malloc.h"

#include <stddef.h>
#include <stdint.h>

static heap_header *seg_list[SEG_LISTS_NUM];

void my_memset(void *dest, int val, size_t len) {
    unsigned char *ptr = dest;
    while (len-- > 0) *ptr++ = val;
}

void heap_init(void *start) {
    uint32_t block_size = MAX_BLOCK_SIZE;
    uint32_t block_num;
    int8_t i = SEG_LISTS_NUM;

    heap_header *header;
    header = (heap_header *)start;

    while (--i >= 0) {
        seg_list[i] = header;
        my_memset((void *)header, 0, sizeof(heap_header));
        block_num = MEM_PER_SEG / block_size;
        heap *hp = (heap *)(((void *)header) + sizeof(heap_header));
        header->block_size = block_size;
        header->head = hp;

        while (block_num--) {
            hp->next = (heap *)(((void *)hp) + block_size);
            my_memset((void *)hp->next, 0, sizeof(heap));
            hp = hp->next;
            hp->next = NULL;
        }

        header += MEM_PER_SEG / sizeof(heap_header);
        block_size /= 2;
    }
}

void *malloc(uint32_t nbytes) {
    int8_t i = 0;
    heap* hp;
    nbytes += sizeof(heap);
    /*First fit*/
    while (i < SEG_LISTS_NUM) {
        if (seg_list[i]->block_size >= nbytes) {
            hp = seg_list[i]->head;
            while (hp) {
                if (!hp->allocated){
                    hp->allocated = 1;
                    return hp++; /* skip next pointer & allocated mem space*/
                }
                hp = hp->next;
            }
        }
        ++i;
    }
    return NULL;
}

void free(void *ptr) {
    if (!ptr) return; /* avoid NULL pointer*/
    heap *hp;
    hp = (heap *)(ptr - sizeof(heap));
    hp->allocated = 0;
}
