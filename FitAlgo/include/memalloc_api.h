#ifndef _MA_MEMALLOC_API_H
#define _MA_MEMALLOC_API_H

#include "block.h"
#include "raf_params.h"
#include "colors.h"
//--------------------------- Defines --------------------------------//

// 100 MB in B
#define MAINBLOCKSIZE 1000000
// Number of Objects
#define OBJECTNUMBER 10000
// Maximum number of holes that can be created
#define MAX_HOLES 52428800
// maximum number of allocation/deallocation operations
#define AFNUMBER 300000

//--------------------------- Methods -------------------------------//

// Method to allocate main block to heap
void *AllocMainBlock();

// Menu Method
int Menu();

// Statistics
void *Statistics();

// Method to generate random objects
void GenerateRandomSizes(size_t *array);

// Method to random allocate/free all sizes in the given array using the given algorithm
void *RandomAllocFree(void *arg);

// Method to print the contents of a block
void PrintBlock(struct Block *block);

// Method to test if the summed sizes of all blocks are equal to MAINBLOCKSIZE
int ValidateBlocks();

// Custom Free Method
void FreeMemory(struct Block *object);

// Custom Alloc Method
struct Block *AllocMemory(struct Block *currentHole, size_t processSize);

// Method used to safetly deallocate (literally) all used pointers
void DestroyFitAlgo();

//---------------------- Allocation Algorithms ----------------------//

struct Block *FirstFit(size_t processSize);
struct Block *BestFit(size_t processSize);
struct Block *NextFit(size_t processSize);
struct Block *WorstFit(size_t processSize);
struct Block *RandomFit(size_t processSize);

#endif