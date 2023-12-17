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

    // for(int i=0;i<OBJECTNUMBER;++i){

    //     firstFit(vSizesToAllocate[i]);
        
    // }
    struct Block* blk[100];
    blk[0]=firstFit(104857575);
    blk[1]=firstFit(20);
    blk[2]=firstFit(5);
    
    // printf("%p  %p\n",blk[1]->previous,blk[0]);

    freeMemory(blk[0]);
    freeMemory(blk[2]);

    printf("\n\n\nObjects\n\n");
    printGivenBlock(firstObject);
    printf("\n\n\nHoles\n\n");
    printGivenBlock(firstHole);
    
    

    free(memory);
    exit(0);
}
