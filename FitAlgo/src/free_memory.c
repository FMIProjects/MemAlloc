#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/block.h"

extern struct Block* firstHole;
extern struct Block* firstObject;
extern void * memory;

void freeMemory(struct Block* object){


    struct Block* previousObject = object-> previous;
    struct Block* nextObject = object-> next;

    // take the next and previous objects of the object that is deallocated and connect them directly 
    if(previousObject!=NULL)
        previousObject->next = nextObject;

    if(nextObject!=NULL)
        nextObject-> previous = previousObject;

    //the current object just becomes a hole so it needs to be added to the proper position into the hole list

    //by using this pointer we will iterate through the hole list
    // it is needed to find the previous and next holes of the object that will be deallocated
    struct Block* currentHole=firstHole;
    struct Block* previousHole=NULL;
    struct Block* nextHole=NULL;

    while(currentHole!=NULL){

        if(currentHole->startAddress < object->startAddress){

            previousHole = currentHole;
            nextHole = currentHole->next;
        }
        else{
            nextHole = currentHole;
            break;
        } 

        currentHole = currentHole->next;

    }

    //seems like lots of cases


}