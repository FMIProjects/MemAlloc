#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../include/fitalgo_api.h"

extern struct Block *firstHole;
extern struct Block *firstObject;
extern struct Block *lastHole;

extern void *memory;

// this needs to be fixed
struct Block *NextFit(size_t processSize)
{
    static size_t lastAllocatedAddress = 0;

    // number of holes
    size_t holeCount = 0;

    // checker if we find a hole
    bool holeFound = false;

    // by using this pointer we will iterate through the hole list
    struct Block *currentHole = firstHole;

    struct Block *firstHoleBeforeLast = NULL;

    // determine the next hole after the last allocated address
    while (currentHole != NULL)
    {
        // determine the first hole before the last memory adress
        // if there is no proper hole after the last allocated address then proper hole will be firstHoleBeforeLast
        if (firstHoleBeforeLast == NULL &&
            currentHole->startAddress < lastAllocatedAddress &&
            currentHole->size >= processSize)
        {

            firstHoleBeforeLast = currentHole;
        }

        // test if the hole is large enough
        if (currentHole->startAddress >= lastAllocatedAddress && currentHole->size >= processSize)
            break;

        currentHole = currentHole->next;
    }

    // if there was no hole found before and after the lastAllocatedAddress it means that there is no large enough hole
    if (currentHole == NULL && firstHoleBeforeLast == NULL)
        return NULL;

    // if there is no hole after the last allocated address then the proper hole is firstHoleBeforeLast
    if (currentHole == NULL)
        currentHole = firstHoleBeforeLast;

    // update the last allocated address
    lastAllocatedAddress = currentHole->startAddress;

    // Object Allocation
    struct Block *newObject = AllocMemory(currentHole, processSize);

    return newObject;
}