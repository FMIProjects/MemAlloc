#include <stdio.h>
#include <stdlib.h>

#include "../include/block.h"

extern struct Block* firstHole;
extern struct Block* firstObject;
extern void * memory;

struct Block* firstFit(size_t processSize){
    
    // using this pointer we will iterate through the hole list
    struct Block* currentHole= firstHole;

    // search into the list the first large enough hole 
    while(currentHole!=NULL){
        
        if(currentHole-> size >= processSize)
            break;
        
        currentHole=currentHole->next;
    }

    // if it was not found return null
    if(currentHole==NULL)
        return NULL;

    // create a new allocated block
    struct Block* newObject = (struct Block*)malloc(sizeof(struct Block));
    newObject->size = processSize;
    newObject->startAddress = currentHole->startAddress; 

    // find needed blocks
    struct Block* previousHole = currentHole->previous;
    struct Block* nextHole = currentHole -> next;
    struct Block* previousObject;
    struct Block* nextObject;

    // using this pointer we will iterate through the object list
    struct Block* currentObject = firstObject;

    // find the allocated objects that are between the current hole
    while(currentObject!=NULL){

        if(currentObject->startAddress < currentHole->startAddress){
            previousObject = currentObject;
            nextObject = currentObject->next;
        }
        else break;

        currentObject = currentObject->next;

    }

    // place the new object between the found objects
    previousObject->next = newObject;
    
    newObject->previous = previousObject;
    newObject->next = nextObject;

    nextObject->previous = newObject;

    //modify or delete current Hole

    // the hole dissapears when it has exactly the same size as the requested process size
    if(currentHole->size == processSize){

        previousHole->next = nextHole;
        nextHole->previous = previousHole;

        free(currentHole);

    }
    // the hole shrinks when the requested process size is less than the hole size
    else{

        currentHole->size -= processSize;
        currentHole->startAddress += processSize;

    }

    return newObject;
}