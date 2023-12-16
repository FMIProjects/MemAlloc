#include "../include/memalloc_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void *AllocMainBlock()
{
    void *memoryBlock = malloc(MAINBLOCKSIZE);

    if (memoryBlock == NULL)
    {
        perror("Main Block Allocation");
        return NULL;
    }

    return memoryBlock;
}

void GenerateRandomSizes(size_t *array)
{
    srand(time(NULL));
    for (int i = 0; i < OBJECTNUMBER; i++)
    {
        // Generate a random size for each object [1,1024] bytes
        size_t objectSize = (rand() % 1024) + 1;
        array[i] = objectSize;
    }
}

/*
void GenerateRandomBlocks()
{

}
*/

