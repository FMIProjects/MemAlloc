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

    // the number of bytes between the main object and the previous and next ones
    size_t distanceNext = nextObject->startAddress - object->size - object->startAddress;
    size_t distancePrevious = previousObject-> startAddress + previousObject->size - object->startAddress;

    //treat the cases when there is no hole between the blocks
    if(nextObject!=NULL && previousObject==NULL && distanceNext==0){

    }else if(nextObject==NULL && previousObject!=NULL && distancePrevious==0){

    }else if(nextObject!=NULL && previousObject!=NULL && distancePrevious==0 && distanceNext==0){

    }


    // case when there is no previous hole
    if(nextHole!=NULL && previousHole==NULL){

        nextHole->startAddress = object->startAddress;
        nextHole->size += object->size;
    }

    // case when there is no next hole
    if(previousHole!=NULL && nextHole==NULL){

        previousHole->size += object->size;
    }

    // case when there are both holes, merge them

    if(previousHole!=NULL && nextHole!=NULL){

        previousHole->size += object->size + nextHole-> size;
        previousHole->next = nextHole->next;
    }


    free(object);
}