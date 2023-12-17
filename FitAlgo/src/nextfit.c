#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../include/memalloc_api.h"

extern struct Block *firstHole;
extern struct Block *firstObject;
extern struct Block *lastHole;

extern void *memory;

// this needs to be fixed
struct Block *NextFit(size_t processSize)
{
    // number of holes
    size_t holeCount = 0;

    size_t validHoleCount = 0;

    // checker if we find a hole
    bool holeFound = false;

    // by using this pointer we will iterate through the hole list
    struct Block *currentHole = firstHole;

    // determine the number of holes
    while (currentHole != NULL)
    {
        holeCount++;
        currentHole = currentHole->next;
    }

    // if it's not the first hole we start searching from the last hole
    // position found
    if (lastHole != NULL)
        currentHole = lastHole;

    while (currentHole != NULL)
    {
        if (currentHole->size >= processSize)
        {
            holeFound = true;
            break;
        }
        currentHole = currentHole->next;
        validHoleCount++;
    } 

    // if it can't find a hole it starts from the first hole again
    if (validHoleCount != holeCount && holeFound == false)
    {
        currentHole = firstHole;
        while (currentHole != lastHole)
        {
            if (currentHole->size >= processSize)
                break;
            currentHole = currentHole->next;
        }
    }

    // modify the lastHole position
    if(currentHole!=NULL && currentHole->next!=NULL)
		lastHole = lastHole->next;
	else
		lastHole = firstHole;

    // if it was not found return null
    if (currentHole == NULL)
        return NULL;

    //printf("%d  %d\n",holeCount,validHoleCount);
    // Object Allocation
    struct Block *newObject = AllocMemory(currentHole, processSize);

    return newObject;
}