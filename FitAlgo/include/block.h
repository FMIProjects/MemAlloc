#ifndef _MA_BLOCK_H
#define _MA_BLOCK_H

#include<stddef.h>

struct Block{
	size_t size;
	size_t startAddress;
	struct Hole *next;
	struct Hole *previous;
};

#endif