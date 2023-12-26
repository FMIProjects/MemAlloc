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
    */
    
    int order;
	size_t size;
	size_t startAddress;
	struct BuddyBlock *next;
	struct BuddyBlock *previous;
};

#endif