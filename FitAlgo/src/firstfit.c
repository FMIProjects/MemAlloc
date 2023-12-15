#include <stdio.h>
#include <stdlib.h>

#include "../include/hole.h"
#include "../include/object.h"

struct Object* firstFit(size_t processSize, void* mainMemory , struct Object* objectToBlock,struct Hole* currentHole){
    
    // search for a large enough block
    while(currentHole!=NULL){

        if(currentHole->size>=processSize)
            break;
        currentHole=currentHole->next;
    }

    if(currentHole==NULL)
        return NULL;
    
    // the new object will start at the hole address
    objectToBlock->startAddress=currentHole->startAddress;
    objectToBlock->size=processSize;
}