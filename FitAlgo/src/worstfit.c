#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/fitalgo_api.h"

extern struct Block *firstHole;
extern struct Block *firstObject;
extern void *memory;

struct Block *WorstFit(size_t processSize)
{
    // by using this pointer we will iterate through the hole list
    struct Block *currentHole = firstHole;
    struct Block *worstHole = NULL;

    size_t maxSize = 0;

    while (currentHole != NULL)
    {

        if (currentHole->size >= maxSize && currentHole->size >= processSize)
        {
            maxSize = currentHole->size;
            worstHole = currentHole;
        }
        currentHole = currentHole->next;
    }

    currentHole = worstHole;

    if (currentHole == NULL)
        return NULL;

    // Object Allocation
    struct Block *newObject = AllocMemory(currentHole, processSize);

    return newObject;
}