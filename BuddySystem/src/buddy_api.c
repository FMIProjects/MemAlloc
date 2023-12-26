#include "../include/buddy_api.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

    while((order+1)*low < size)
        ++order;

    return order;
    
}


int buddyInit(size_t size, size_t low, size_t high){

    if (!( isPowerOfTwo(size) && isPowerOfTwo(low) && isPowerOfTwo(high)) )
        return -1;

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

struct BuddyBlock* BuddyAlloc(size_t size){

    int currentOrder = calculateOrder(size, minimumSize);
    
}