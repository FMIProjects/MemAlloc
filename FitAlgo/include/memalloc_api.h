#ifndef _MA_MEMALLOC_API_H
#define _MA_MEMALLOC_API_H

#include "block.h"

// 100 MB in B
#define MAINBLOCKSIZE 104857600
// Number of Objects
#define OBJECTNUMBER 10000
//Maximum number of holes that can be created
#define MAX_HOLES 52428800
 
// method to allocate main block to heap
void *AllocMainBlock();

// method to generate random objects
void GenerateRandomSizes(size_t *array);

// method to generate random blocks that add up to MAINBLOCKSIZE
//void GenerateRandomBlocks();

#endif