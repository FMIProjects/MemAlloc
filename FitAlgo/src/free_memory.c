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

    
    // remeinder to treat cases when there are no holes and there are more blocks near one anothers

    //treat the cases when there is no hole between the blocks

    //case when there are 2 blocks near each other and the one being deallocated is the one on the left
    if(nextObject!=NULL && nextHole->startAddress > nextObject->startAddress){

        previousHole->size += object->size;

        if(previousObject!=NULL){

            nextObject->previous = previousObject;
            previousObject->next = nextObject;
        }
        
        free(object);
        return;


    }
    //case when there are 2 blocks near each other and the one being deallocated is the one on the right
    else if(nextObject==NULL && previousObject!=NULL && previousHole->startAddress < previousObject->startAddress){

        nextHole->size += object->size;
        nextHole-> startAddress = object->startAddress;

        if(nextObject!=NULL){
            nextObject->previous = previousObject;
            previousObject->next = nextObject;
        }

        free(object);
        return;

    }
    //case when there are more than 2 blocks near one another
    else if(nextObject!=NULL && previousObject!=NULL &&
            nextHole->startAddress > nextObject->startAddress &&
            previousHole->startAddress < previousObject->startAddress){

        nextObject->previous = previousObject;
        previousObject->next = nextObject;

        // no need to deallocate the block of memory, it will just become a hole

        previousHole->next = object;
        object -> previous = previousHole;
        object -> next = nextHole;
        nextHole -> previous = object;

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