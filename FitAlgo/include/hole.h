#ifndef _MA_HOLE_H
#define _MA_HOLE_H

#include<stddef.h>

struct Hole{
	size_t size;
	size_t startAddress;
	struct Hole *next;
	struct Hole *previous;
};

#endif