#ifndef _MA_MEMALLOC_API_H
#define _MA_MEMALLOC_API_H

#include "block.h"

//--------------------------- Defines --------------------------------//

// 100 MB in B
#define MAINBLOCKSIZE 104857600
// Number of Objects
#define OBJECTNUMBER 10000
// Maximum number of holes that can be created
#define MAX_HOLES 52428800
// maximum number of allocation/deallocation operations
#define AFNUMBER 10000

//--------------------------- Methods -------------------------------//

// Method to allocate main block to heap
void *AllocMainBlock();

// Method to generate random objects
void GenerateRandomSizes(size_t *array);

// Method to allocate all sizes in the given array using the given algorithm
void AllocateSizes(size_t *array, const char * const Algo);

// Method to random allocate/free all sizes in the given array using the given algorithm
void RandomAllocFree(size_t *array, const char * const Algo);

// Method to print the contents of a block
void PrintBlock(struct Block *block);

// Custom Free Method
void FreeMemory(struct Block *object);

// Custom Alloc Method
struct Block *AllocMemory(struct Block *currentHole, size_t processSize);

//---------------------- Allocation Algorithms ----------------------//

struct Block *FirstFit(size_t processSize);
struct Block *BestFit(size_t processSize);
struct Block *NextFit(size_t processSize);
struct Block *WorstFit(size_t processSize);
struct Block *RandomFit(size_t processSize);

#endif