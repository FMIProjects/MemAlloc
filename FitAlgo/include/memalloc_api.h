#ifndef _MA_MEMALLOC_API_H
#define _MA_MEMALLOC_API_H

#include "object.h"
#include "hole.h"

// 100 MB in B
#define MAINBLOCKSIZE 104857600
// Number of Objects
#define OBJECTNUMBER 10000

// method to allocate main block to heap
void *AllocMainBlock();

// method to generate random objects
void GenerateRandomObjects(struct Object *array);

// method to generate random blocks that add up to MAINBLOCKSIZE
//void GenerateRandomBlocks();

#endif