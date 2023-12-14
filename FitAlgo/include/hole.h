#ifndef _MA_HOLE_H
#define _MA_HOLE_H

struct Hole
{	
	int size;
	int startAddress;
	struct Hole *next;
	struct Hole *previous;
};

#endif