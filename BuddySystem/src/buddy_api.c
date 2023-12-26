#include "../include/buddy_api.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>


//--------------------------- Declarations --------------------------------//

extern size_t maxMemorySize;
extern size_t minimumSize;
extern size_t maximumSize;


int isPowerOfTwo(size_t n){
    return (n > 0) && ( (n & (n-1)) == 0);
}


int buddyInit(size_t size, size_t low, size_t high){

    if (!( isPowerOfTwo(size) && isPowerOfTwo(low) && isPowerOfTwo(high)) )
        return -1;

    maxMemorySize = size;
    minimumSize = low;
    maximumSize = high;

}