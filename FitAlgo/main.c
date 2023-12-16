#include <stdio.h>
#include <stdlib.h>

#include "include/memalgo.h"
#include "include/memalloc_api.h"

#include "include/block.h"

size_t vSizesToAllocate[OBJECTNUMBER];

/*
    Fun fact!
    the maximum number of holes in a memory of and even size of bytes is that size of the whole memory
    divided by 2 (first bytes is allocated, the second is not, the third is and so on). In this case the
    maximum number of Holes is 100mb/2 in bytes ~= 52 milion bytes. A single hole takes 32 bytes. So
    the vector of holes needs to be like 1.6 billion bytes => 1.5 Gb of memory.
*/

struct Block* firstHole = NULL;

struct Block* firstObject = NULL;

void * memory;

int main()
{
    memory = AllocMainBlock();

    //create the first hole which will be the whole memory
    firstHole = (struct Block*) malloc(sizeof(struct Block));
    firstHole-> size = MAINBLOCKSIZE;
    firstHole-> startAddress = 0;
    firstHole-> previous=NULL;
    firstHole-> next=NULL;

    GenerateRandomSizes(vSizesToAllocate);

    //GenerateRandomBlocks();
    
    // //output random objects
    // for (int i = 0; i < OBJECTNUMBER; i++)
    // {
    //     printf("%d : %zu \n",i,vSizesToAllocate[i]);
    // }


    firstFit(20);
    firstFit(5);
    firstFit(87);
    
    printf("Holes\n\n");
    printGivenBlock(firstHole);
    printf("\n\n\nObjects\n");
    printGivenBlock(firstObject);

    free(memory);
    exit(0);
}
