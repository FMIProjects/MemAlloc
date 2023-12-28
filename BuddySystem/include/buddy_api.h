#ifndef _MA_BUDDY_API_H
#define _MA_BUDDY_API_H

#endif

#include "buddyblock.h"
//--------------------------- Defines --------------------------------//

// one MB is 1048576 (2^20) bytes => 128 MB is 134217728 (2^27) bytes
#define MAINBLOCKSIZE 134217728
// the minimum size that can be allocated is 1024 (2^10) bytes => 1MB/1024B
#define MINIMUMSIZE 1024
// the maximum size that can be allocated is 64 MB => 67108864 (2^26) bytes
#define MAXIMUMSIZE 67108864
// Number of Objects
#define OBJECTNUMBER 10000
// number of allocation/deallocation operations
#define AFNUMBER 300000

//-------------------- Buddy Specific Methods ------------------------//

// Method used to initialise the size and bounds of the allocator
int BuddyInit(size_t size, size_t low, size_t high);

// Method used to allocate memory using the buddy system
struct BuddyBlock *BuddyAlloc(size_t size);

// Method used to deallocate memory from the buddy system
void FreeBuddyMemory(struct BuddyBlock *object);

// Method used to partition of hole till one the order equal to stopOrder is found, returns the first Hole with the order equal to stopOrder
struct BuddyBlock *partitionHole(struct BuddyBlock *chosenHole, int stopOrder);

// Method used to find the buddy of a block and return it if it is a hole of the same order
struct BuddyBlock *FindBuddy(struct BuddyBlock *hole);

// Method used to merge the given hole with the holes on the right if they can be merged
struct BuddyBlock *MergeHoles(struct BuddyBlock *hole);

// Method used to deallocate all buddy objects
void BuddyReset();

// Method used to destroy the buddy workspace
void BuddyDestroy();

// Method used to calculate the addres of the buddy of the given block
size_t FindBuddyAddress(struct BuddyBlock *block);

// Method used for buddy init
int isPowerOfTwo(size_t n);

// Method to calculate the order of a requested size
int calculateOrder(size_t size, size_t low);

//--------------------------- Utils Methods --------------------------------//

// Menu Method
void Menu();

// Method used to print the whole Objects or Holes
void PrintBlock(struct BuddyBlock *block);

// Method used to test if the sizes of the objects and holes are valid
int ValidateBlocks();

// Statistics Method
void *Statistics();

// Method to random allocate/free blocks for buddy
void *RandomAllocFree(void *arg);

// Method to generate random objects
void GenerateRandomSizes(size_t *array);