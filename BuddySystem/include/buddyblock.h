#ifndef _MA_BUDDYBLOCK_H
#define _MA_BUDDYBLOCK_H

#include <stddef.h>
// Used for Objects and Holes in Buddy allocator

// sizeof = 40
struct BuddyBlock
{   
    /*
    the minimum order is 0. 
    the maximum order is obtained from the formula: 2^order * MINIMUMSIZE = MAXBLOCKSIZE


    !!!CONVENTION!!!

    Hole 
    If the block is a Hole the size is equal to the whole block size: 2^order * MINIMUMSIZE

    Object
    If the block is an Object the size is equal to the size of allocated bytes. So the internal fragemntation is equal to
    2^order * MINIMUMSIZE - size
    */
    

    int order;
	size_t size;
	size_t startAddress;
	struct BuddyBlock *next;
	struct BuddyBlock *previous;
};

#endif