#include "../include/buddy_api.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

//--------------------------- Declarations --------------------------------//

extern size_t maxMemorySize;
extern size_t minimumSize;
extern size_t maximumSize;

extern struct BuddyBlock* firstHole;
extern struct BuddyBlock* firstObject;

//------------------------------ Methods ----------------------------------//

int isPowerOfTwo(size_t n){
    return (n > 0) && ( (n & (n-1)) == 0);
}

int calculateOrder(size_t size, size_t low){

    int order=0;

    while((1<<order)*low < size)
        ++order;

    return order;
    
}


int buddyInit(size_t size, size_t low, size_t high){

    // if the sizes are not of power of 2 return -1
    if (!( isPowerOfTwo(size) && isPowerOfTwo(low) && isPowerOfTwo(high)) )
        return -1;
    
    // initialize the sizes
    maxMemorySize = size;
    minimumSize = low;
    maximumSize = high;

    // initialize the first hole
    firstHole = (struct BuddyBlock* )malloc(sizeof(struct BuddyBlock));

    firstHole->next = NULL;
    firstHole->previous = NULL;
    firstHole->size = maxMemorySize;
    firstHole->startAddress = 0;
    firstHole->order = log2((double)size/low);

    return 0;
}

int partitionHole(struct BuddyBlock* chosenHole, int stopOrder){

    if(chosenHole==NULL || stopOrder>= chosenHole->order)
        return -1;

    struct BuddyBlock* previousHole = chosenHole->previous;
    struct BuddyBlock* nextHole = chosenHole->next;

    struct BuddyBlock* newHole = NULL;
    struct BuddyBlock* currentHole = chosenHole;

    while(currentHole->order != stopOrder){

        
        //create a newHole
        newHole = (struct BuddyBlock*)malloc(sizeof(struct BuddyBlock));
        newHole->startAddress = currentHole->startAddress;
        newHole->size = currentHole->size >> 1;
        newHole->next = currentHole;
        newHole->previous = NULL;
        newHole-> order = currentHole->order -1;

        //split the current Hole into half
        currentHole->size = currentHole->size >>1;
        currentHole-> previous = newHole;
        currentHole-> order -=1;
        currentHole->startAddress += currentHole->size;

        currentHole = newHole;
    }

    // connect the new Hole to the previous hole of the now split chosen hole
    newHole->previous = previousHole;

    // also connect the previous Hole to the newHOle
    if(previousHole != NULL)
        previousHole->next= newHole;

    // also get back the firstHole

    while(firstHole->previous != NULL)
        firstHole = firstHole -> previous;

    return 1;

}

struct BuddyBlock* BuddyAlloc(size_t size){

    int currentOrder = calculateOrder(size, minimumSize);

    // by using this pointer we will iterate through the hole blocks
    struct BuddyBlock* currentHole = firstHole;

    // first of all we shall search if there is a suitable block of the same order as the requested size
    while(currentHole!= NULL){
        
        if(currentHole-> order == currentOrder)
            break;

        currentHole = currentHole-> next;
    }

    // case when there is a suitable hole
    if(currentHole!=NULL){
        
        // connect the next and previous holes if they exist
        struct BuddyBlock* nextHole = currentHole->next;
        struct BuddyBlock* previousHole = currentHole->previous;

        if(previousHole != NULL)
            previousHole->next = nextHole;
        
        if(nextHole != NULL){
            nextHole->previous = previousHole;
        }

        // update the firstHole if necessary
        if(currentHole==firstHole)
            firstHole = nextHole;
           

        //sanitise the currentHole, it will become an Object

        currentHole->previous = NULL;
        currentHole->next = NULL;
        currentHole->size = size; 

        // case when there is no first object
        if(firstObject==NULL){
            firstObject = currentHole;
            return firstObject;
        }

        //case when the objects need to be connected to other objects

        struct BuddyBlock* nextObject;
        struct BuddyBlock* previousObject;
        struct BuddyBlock* currentObject = firstObject;

        // we will find the suitable objects by comparing strat addresses

        while(currentObject!= NULL){

            if(currentObject->startAddress < currentHole->startAddress){

                previousObject = currentObject;
                nextObject = currentObject->next;
            }
            else{

                nextObject = currentObject;
                break;
            }

            currentObject = currentObject->next;
        }

        // connect the allocated object ( currentHole ) to the found objects ifn they exist
        if(previousObject != NULL){

            previousObject->next = currentHole;
            currentHole->previous = previousObject;
        }

        if(nextObject != NULL){

            currentHole->next = nextObject;
            nextObject->previous = currentHole; 
        }
    }

    // case when there is no suitable hole
    // we need to find the last hole that has the minimum order that is bigger than the order of the current object


    struct BuddyBlock* chosenHole = NULL;

    // we reuse the currentHole pointer to iterate through the holes
    currentHole=firstHole;
    int minOrder = INT_MAX;

    while(currentHole!=NULL){
        
        // look for the last hole with the minimum order that is bigger than the order of the current object
        if(minOrder <= currentHole->order && currentHole->order > currentOrder){

            minOrder = currentHole -> order;
            chosenHole = currentHole;
        }
        
        currentHole= currentHole->next;

    }

    // if the memory is overloaded and cannot have a block of the given size return null
    if(chosenHole == NULL)
        return NULL;

    // if we have found a Hole we need to start split it into partitons with lower orders till we get one with the order equal to the current order

}



void PrintBlock(struct BuddyBlock *block)
{

    struct BuddyBlock *currentBlock = block;

    int blockNumber = 1;

    if(block == firstObject){

        printf("Objects\n");

        while (currentBlock != NULL)
        {
            size_t wholeBlockSize = (size_t)pow(2,currentBlock->order) * minimumSize;

            printf("-----------------------------\n");
            printf("Block number: %d \n", blockNumber);
            printf("Start address of block: %zu \n", currentBlock->startAddress);
            printf("Block order: %d \n", currentBlock->order);
            printf("Allocated size: %zu \n", currentBlock->size);
            printf("Whole block size: %zu \n",wholeBlockSize);
            printf("Internal fragmentation: %zu \n",wholeBlockSize-currentBlock->size);
            printf("-----------------------------\n");

            currentBlock = currentBlock->next;
            ++blockNumber;
        }

    }

    if(block == firstHole){

        printf("Holes\n");
        while (currentBlock != NULL)
        {
            
            printf("-----------------------------\n");
            printf("Block number: %d \n", blockNumber);
            printf("Start address of block: %zu \n", currentBlock->startAddress);
            printf("Block order: %d \n", currentBlock->order);
            printf("Hole size: %zu \n", currentBlock->size);
            printf("-----------------------------\n");

            currentBlock = currentBlock->next;
            ++blockNumber;
        }

    } 

}

int ValidateBlocks()
{
    size_t summedSizes = 0;

    struct BuddyBlock *currentBlock = firstObject;

    while (currentBlock != NULL)
    {
        summedSizes += (1<<currentBlock->order) * minimumSize;
        currentBlock = currentBlock->next;
    }

    currentBlock = firstHole;

    while (currentBlock != NULL)
    {
        summedSizes += currentBlock->size;
        currentBlock = currentBlock->next;
    }

    return (summedSizes == maxMemorySize);
}