#ifndef _MA_BLOCK_H
#define _MA_BLOCK_H

#include <stddef.h>

// Used for Objects and Holes

struct Block
{
	size_t size;
	size_t startAddress;
	struct Block *next;
	struct Block *previous;
};

#endif