#include <stdio.h>
#include <stdlib.h>

#include "include/memalgo.h"
#include "include/memalloc_api.h"

#include "include/object.h"
#include "include/hole.h"

struct Object vObject[OBJECTNUMBER * sizeof(struct Object)];

int main()
{
    void *memory = AllocMainBlock();

    GenerateRandomObjects(vObject);

    //GenerateRandomBlocks();
    
    //output random objects
    for (size_t i = 0; i < OBJECTNUMBER; i++)
    {
        printf("%d : %d \n",i,vObject[i].size);
    }

    free(memory);
    exit(0);
}
