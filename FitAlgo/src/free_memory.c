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

    // the current object will become a hole so it is safe to sanitize it
    object->previous=NULL;
    object->next = NULL;

    // take the next and previous objects of the object that is deallocated and connect them directly 
    if(previousObject!=NULL){
        previousObject->next = nextObject;

        // modify the first object if the object to deallocate is actuallly the first object
        if(firstObject==object)
            firstObject = previousObject;
    }
        

    if(nextObject!=NULL){

        nextObject-> previous = previousObject;

        // modify the first object into the next object if there is no previous object
        if(firstObject == object && previousObject==NULL)
            firstObject = nextObject;
    }

    // if the object to deallocate is the only object just make the first object empty
    if(nextObject==NULL && previousObject == NULL ){
        firstObject = NULL;
    }

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

    

    // case when the memory is full and there is no Hole

    if(nextHole==NULL&& previousHole==NULL){

        // no need to deallocate we will use the effective pointer
        firstHole = currentHole;
        firstHole->next = NULL;
        firstHole->previous = NULL;

        return;
        
    }


    if(previousHole!=NULL){
        previousHole->next = object;
        object->previous = previousHole;

        // test if the holes can be merged
        if(previousHole->startAddress + previousHole-> size == object->startAddress){

            object->size += previousHole->size;
            object-> startAddress = previousHole->startAddress;
            object -> previous = previousHole->previous;

            if(previousHole==firstHole)
                firstHole=object;
            // no need to keep it anymore
            free(previousHole);
        }

    }

    if(nextHole!=NULL){

        object->next = nextHole;
        nextHole->previous = object;

        //test if the holes can be merged

        if(object->startAddress + object->size == nextHole->startAddress){

            object->size += nextHole->size;
            object->next = nextHole->next;

            if(nextHole==firstHole)
                firstHole=object;
            // no need to keep it anymore
            free(nextHole);

        }


    }

    // case when the nextHole is the firstHole
    while(firstHole->previous !=NULL){
        firstHole = firstHole->previous;
    }


}