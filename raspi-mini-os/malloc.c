// TODO : add sbrk function to manage heap
// TODO : add merge and split while keep memory alignment
#include "malloc.h"
#include <stddef.h>
#include <stdint.h>

static heap_header* seg_list[SEG_LISTS_NUM];

void my_memset(void* dest, int val, size_t len)
{
    unsigned char* ptr = dest;
    while (len-- > 0)
        *ptr++ = val;
}

void heap_init(void* start)
{
    uint32_t block_size = 16;
    uint32_t block_num;
    int8_t i = 0;

    heap_header* header;
    header = (heap_header*)start;

    while (i < SEG_LISTS_NUM) {
        seg_list[i] = header;
        my_memset((void*)header, 0, sizeof(heap_header));
        block_num = MEM_PER_SEG / block_size;
        Heap* hp = (Heap*)(((void*)header) + sizeof(heap_header));
        header->block_size = block_size;
        header->head = hp;

        while (block_num--) {
            hp->next = (Heap*)(((void*)hp) + block_size);
            my_memset((void*)hp->next, 0, sizeof(Heap));
            hp = hp->next;
            hp->next = NULL;
        }
        header->tail = hp;
        header += MEM_PER_SEG / sizeof(heap_header); 
        block_size *= 2;
    }
}

/*  Implement FIFO (First In First Out) policy, 
    only keep free heap segments in list to save space&time,
    malloc take firt segment from head, free append free segment to tail.
*/
void* malloc(uint32_t nbytes)
{
    int8_t i = 0;
    Heap* hp;
    nbytes += sizeof(Heap);
    /*First fit*/
    while (i < SEG_LISTS_NUM) {
        if (seg_list[i]->block_size >= nbytes && seg_list[i]->head) {
            hp = seg_list[i]->head;
            seg_list[i]->head = hp->next;
            return hp++;
        }
        ++i;
    }
    return NULL;
}

void free(void* ptr)
{
    if (!ptr)
        return;

    int32_t index;
    Heap* hp;
    hp = (Heap*)(ptr - sizeof(Heap));
    hp->next = NULL;
    index = (*(int32_t*)ptr - *(int32_t*)seg_list[0]->head) / (int32_t)MEM_PER_SEG;

    if (!seg_list[index]->head) { //if no head then no tail
        seg_list[index]->head = hp;
        seg_list[index]->tail = hp;
        return;
    }
    seg_list[index]->tail->next = hp;
    seg_list[index]->tail = hp;
}
