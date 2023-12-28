#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "include/buddy_api.h"
#include "include/buddyblock.h"
#include <math.h>

size_t vSizesToAllocate[OBJECTNUMBER];

size_t maxMemorySize;
size_t minimumSize;
size_t maximumSize;

struct BuddyBlock *firstHole;
struct BuddyBlock *firstObject;

int main()
{

    if (BuddyInit(MAINBLOCKSIZE, MINIMUMSIZE, MAXIMUMSIZE))
    {
        perror("Invalid allocator parameters\n\a");
        exit(EXIT_FAILURE);
    }
    
    GenerateRandomSizes(vSizesToAllocate);

    GenerateRandomSizes(vSizesToAllocate);

    Menu();
    
    struct BuddyBlock *blk[100];
    blk[0] = BuddyAlloc(16000);
    FreeBuddyMemory(blk[0]);

    PrintBlock(firstObject);
    printf("\n");
    PrintBlock(firstHole);
    printf("%d\n", ValidateBlocks());

    BuddyDestroy();
    exit(0);
}