#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include "include/memalloc_api.h"
#include "include/block.h"

size_t vSizesToAllocate[OBJECTNUMBER];
struct Block *firstHole = NULL;
struct Block *firstObject = NULL;

// Used for NextFit
struct Block *lastHole = NULL;

void *memory;

int algorithm;

int main()
{   
    memory = AllocMainBlock();

    // create the first hole which will be the whole memory
    firstHole = (struct Block *)malloc(sizeof(struct Block));
    firstHole->size = MAINBLOCKSIZE;
    firstHole->startAddress = 0;
    firstHole->previous = NULL;
    firstHole->next = NULL;

    GenerateRandomSizes(vSizesToAllocate);

    algorithm = Menu();

    RandomAllocFree(vSizesToAllocate,algorithm);
    
    /*printf("\n\n\nObjects\n\n");
    PrintBlock(firstObject);
    printf("\n\n\nHoles\n\n");
    PrintBlock(firstHole);

    printf("%d\n" ,ValidateBlocks());
    */
    free(memory);
    exit(0);
}
