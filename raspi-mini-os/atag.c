#include "atag.h"
#include <stdint.h>

uint32_t get_mem_size(atag_t *tag) {
    while (tag->tag_type != ATAG_NONE) {
        if (tag->tag_type == ATAG_MEM) 
            return tag->mem.size;
        tag = (atag_t *) (((uint32_t *)tag) + tag->tag_size);
    }
    return 0;
}
