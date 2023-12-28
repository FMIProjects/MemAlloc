#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/fitalgo_api.h"

extern struct Block *firstHole;
extern struct Block *firstObject;
extern void *memory;

struct Block *FirstFit(size_t processSize)
{

    // by using this pointer we will iterate through the hole list
    struct Block *currentHole = firstHole;

    // search into the list the first large enough hole
    while (currentHole != NULL)
    {
        if (currentHole->size >= processSize)
            break;

        currentHole = currentHole->next;
    }

    // if it was not found return null
    if (currentHole == NULL)
        return NULL;

    // Object Allocation
    struct Block *newObject = AllocMemory(currentHole, processSize);

    return newObject;
}