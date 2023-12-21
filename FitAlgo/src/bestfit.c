#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/memalloc_api.h"

#define INT_MAX 2147483647;

extern struct Block *firstHole;
extern struct Block *firstObject;
extern void *memory;

struct Block *BestFit(size_t processSize)
{
    // by using this pointer we will iterate through the hole list
    struct Block *currentHole = firstHole;
    struct Block *bestHole = NULL;

    size_t minDiff = INT_MAX;
    size_t diff = 0;

    while (currentHole != NULL)
    {
        diff = (currentHole->size - processSize);
        if (diff < minDiff && diff >= 0)
        {
            minDiff = diff;
            bestHole = currentHole;
        }
        currentHole = currentHole->next;
    }

    currentHole = bestHole;

    if(currentHole == NULL)
        return NULL;
    
    // Object Allocation
    struct Block *newObject = AllocMemory(currentHole, processSize);

    return newObject;
}