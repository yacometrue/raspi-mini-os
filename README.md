## Raspi-mini-os
An simple & mini operating system for Raspberry Pi

## Implementation Features
* dynamic allocator
    * Segregated List in malloc.c file
        * Include many size of blocks which contain their own free list
        * How to allocate a block of size n 
            * go through each free list to find appropriate size m, which m > n
            * if no free blocks remain in size m free list, try next larger class
            * Repeat until find a free block, if not found return NULL
        * How to free
            * total memory usage of each size class is same in this implementation,
              so we could simply modified its memory address to get its place in segregated list
        * Conclusion:
            * For malloc: 
                * Time complexity: O(ln n) log time for power of two size classes 
                * approximate best fit if consider memory alignment
            * For free:
                * Little trick here: we assign same total memory size for all size class,
                  so we could modified memory address of block-to-be-free by total memory size,
                  to get block-to-be-free's size class it belong
                * append free block to the tail of free list of same size class  

## Source: 
* [useful info about raspi](https://wiki.osdev.org/Raspberry_Pi_Bare_Bones)
* [nice tutorial](https://jsandler18.github.io/)
