#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/memalloc_api.h"

extern struct Block *firstHole;
extern struct Block *firstObject;
extern void *memory;

struct Block *WorstFit(size_t processSize)
{
    // by using this pointer we will iterate through the hole list
    struct Block *currentHole = firstHole;
    struct Block *worstHole = NULL;

    size_t maxDiff = 0;
    size_t diff = 0;

    while (currentHole != NULL)
    {
        diff = (currentHole->size - processSize);
        if (diff > maxDiff && diff >= 0)
        {
            maxDiff = diff;
            worstHole = currentHole;
        }
        currentHole = currentHole->next;
    }

    currentHole = worstHole;

    // Object Allocation
    struct Block *newObject = AllocMemory(currentHole, processSize);

    return newObject;
}