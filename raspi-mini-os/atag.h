#include <stdint.h>
#ifndef ATAG_H
#define ATAG_H

#define ATAG_NONE 0x00000000
#define ATAG_CORE 0x54410001
#define ATAG_MEM 0x54410002
#define ATAG_VIDEOTEXT 0x54410003
#define ATAG_RAMDISK 0x54410004
#define ATAG_INITRD2 0x54420005
#define ATAG_SERIAL 0x54410006
#define ATAG_REVISION 0x54410007
#define ATAG_VIDEOLFB 0x54410008
#define ATAG_CMDLINE 0x54410009

typedef struct {
    uint32_t size; /* length of tag in words including this header */
    uint32_t tag_type;
} atag_header;

typedef struct {
    uint32_t size;
    uint32_t start;
} atag_mem;

typedef struct {
    uint32_t start;
    uint32_t size;
} atag_initrd2;

typedef struct {
    char cmdline[1];
} atag_cmdline;

typedef struct {
    uint32_t tag_size;
    uint32_t tag_type;
    union {
        atag_mem     mem;
        atag_initrd2 initrd2;
        atag_cmdline cmdline;
    };
} atag_t;

uint32_t get_mem_size(atag_t *atag);

#endif
