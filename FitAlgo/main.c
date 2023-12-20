#include <stdio.h>
#include <stdlib.h>

#include "include/memalloc_api.h"
#include "include/block.h"

size_t vSizesToAllocate[OBJECTNUMBER];
struct Block *firstHole = NULL;
struct Block *firstObject = NULL;

// Used for NextFit
struct Block *lastHole = NULL;

void *memory;

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

    /* First Fit Example
    struct Block *blk[100];
    blk[0] = FirstFit(104857575);
    blk[1] = FirstFit(20);
    blk[2] = FirstFit(5);

    // printf("%p  %p\n",blk[1]->previous,blk[0]);

    FreeMemory(blk[0]);
    FreeMemory(blk[2]);
    */

    /* Next Fit Example*/
    struct Block* blk[100];

    blk[0] = FirstFit(150);

    blk[1] = NextFit(500);
    FreeMemory(blk[0]);

    blk[3] = NextFit(100);
    blk[4] = NextFit(25);

    
    blk[5] = FirstFit(40);
    blk[6] = FirstFit(20);
    for(int i=7;i<100;i++)
        blk[i] = NextFit(20);


    printf("\n\n\nObjects\n\n");
    PrintBlock(firstObject);
    printf("\n\n\nHoles\n\n");
    PrintBlock(firstHole);

    free(memory);
    exit(0);
}
