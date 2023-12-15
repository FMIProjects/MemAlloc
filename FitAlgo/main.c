#include <stdio.h>
#include <stdlib.h>

#include "include/memalgo.h"
#include "include/memalloc_api.h"

#include "include/object.h"
#include "include/hole.h"

struct Object vObject[OBJECTNUMBER];

/*
    the maximum number of holes in a memory of and even size of bytes is that size of the whole memory
    divided by 2 (first bytes is allocated, the second is not, the third is and so on). In this case the
    maximum number of Holes is 100mb/2 in bytes ~= 52 milion bytes. A single hole takes 32 bytes. So
    the vector of holes needs to be like 1.6 billion bytes => 1.5 Gb of memory.
*/

struct Hole vHoles[MAX_HOLES];

 // the number of holes that are currently in the memory, at start the whole memory space is a hole
int numberHoles=1;

int main()
{
    void *memory = AllocMainBlock();
    
    

    GenerateRandomObjects(vObject);

    //GenerateRandomBlocks();
    
    //output random objects
    for (int i = 0; i < OBJECTNUMBER; i++)
    {
        printf("%zu : %zu \n",i,vObject[i].size);
    }

   

    free(memory);
    exit(0);
}
