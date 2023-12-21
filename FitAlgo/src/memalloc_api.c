#include "../include/memalloc_api.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

//--------------------------- Declarations --------------------------------//

extern struct Block *firstHole;
extern struct Block *firstObject;
extern void *memory;

//------------------------------ Methods ----------------------------------//

void *AllocMainBlock()
{
    void *memoryBlock = malloc(MAINBLOCKSIZE);

    if (memoryBlock == NULL)
    {
        perror("Main Block Allocation");
        return NULL;
    }

    return memoryBlock;
}

void GenerateRandomSizes(size_t *array)
{

    static unsigned int seed = 0;

    srand(time(NULL) + seed);

    for (int i = 0; i < OBJECTNUMBER; ++i)
    {
        // Generate a random size for each object [1,1024] bytes
        size_t objectSize = (rand() % 1024) + 1;
        array[i] = objectSize;
    }

    seed += 700119;
}

void RandomAllocFree(size_t *array, int Algo)
{
    struct Block *blk[OBJECTNUMBER];
    static unsigned int seed = 0;

    srand(time(NULL) + seed);

    size_t afRandom;
    size_t freeIndexRandom;
    size_t allocIndexRandom;
    size_t indexAlloc = 0;

    for (int i = 0; i < AFNUMBER; i++)
    {
        afRandom = (rand() % 2) + 1;
        if (afRandom == 1)
        {

            allocIndexRandom = (rand() % OBJECTNUMBER) + 1;

            switch (Algo) 
            {
            case 1:
                FirstFit(array[allocIndexRandom]);
                break;
            case 2:
                NextFit(array[allocIndexRandom]);
                break;
            case 3:
                BestFit(array[allocIndexRandom]);
                break;
            case 4:
                WorstFit(array[allocIndexRandom]);
                break;
            default:
                return;
            }

            indexAlloc++;
        }
        else
        {
            if (indexAlloc != 0)
            {
                freeIndexRandom = (rand() % indexAlloc) + 1;
                struct Block *currentObject = firstObject;
                size_t currentIndex = 0;

                // search into the list the first large enough hole
                while (currentObject != NULL)
                {
                    currentIndex++;
                    if (currentIndex == freeIndexRandom)
                        break;
                    
                    currentObject = currentObject->next;
                }

                if(currentObject!=NULL){
                        FreeMemory(currentObject);
                        indexAlloc--;
                }
                
            }
        }
    }
}

void PrintBlock(struct Block *block)
{

    struct Block *currentBlock = block;

    int blockNumber = 1;

    while (currentBlock != NULL)
    {

        printf("-----------------------------\n");
        printf("Block number: %d \n", blockNumber);
        printf("Start address of block: %zu \n", currentBlock->startAddress);
        printf("Size of block: %zu \n", currentBlock->size);
        printf("-----------------------------\n");

        currentBlock = currentBlock->next;
        ++blockNumber;
    }
}

int Menu()
{
    char input[10];
    printf("Choose an algorithm by writing the designated number:\n");
    printf("1 - FirstFit\n");
    printf("2 - NextFit\n");
    printf("3 - BestFit\n");
    printf("4 - WorstFit\n");
    printf("\n");
    printf("Algorithm: ");
    scanf("%s",input);
    int algorithm = atoi(input);
    if(algorithm > 4)
        perror("Wrong Input!"),exit(1);
    return algorithm; 
}

//------------------------------ Free/Alloc/RandomFit Methods -----------------------//

struct Block *RandomFit(size_t processSize)
{

    static unsigned int seed = 0;

    srand(time(NULL) + seed);

    int FitIndex = rand() % 4 + 1;

    seed += 100937;

    switch (FitIndex)
    {

    case 1:
        printf("first\n");
        return FirstFit(processSize);

    case 2:
        printf("next\n");
        return NextFit(processSize);

    case 3:
        printf("worst\n");
        return WorstFit(processSize);

    case 4:
        printf("best\n");
        return BestFit(processSize);

    default:
        return NULL;
    }
}

void FreeMemory(struct Block *object)
{
    if(object==NULL)
        return;

    // obtain the previous and next objects of the object that will be deallocated
    struct Block *previousObject = object->previous;
    struct Block *nextObject = object->next;

    // the current object will become a hole so it is safe to sanitize it
    object->previous = NULL;
    object->next = NULL;

    // take the next and previous objects of the object that is deallocated and connect them directly if possible
    if (previousObject != NULL)
    {
        previousObject->next = nextObject;

        // modify the first object if the object to deallocate is actuallly the first object
        // if (firstObject == object)
        //     firstObject = previousObject;
    }

    if (nextObject != NULL)
    {

        nextObject->previous = previousObject;

        // modify the first object into the next object if there is no previous object and if the object to deallocate is the first object
        if (firstObject == object && previousObject == NULL)
            firstObject = nextObject;
    }

    // if the object to deallocate is the only object just make the first object empty
    if (nextObject == NULL && previousObject == NULL)
    {
        firstObject = NULL;
    }

    // the current object just becomes a hole so it needs to be added to the proper position into the hole list

    // by using this pointer we will iterate through the hole list

    struct Block *currentHole = firstHole;
    struct Block *previousHole = NULL;
    struct Block *nextHole = NULL;

    // it is needed to find the previous and next holes of the object that will be deallocated
    while (currentHole != NULL)
    {

        if (currentHole->startAddress < object->startAddress)
        {

            previousHole = currentHole;
            nextHole = currentHole->next;
        }
        else
        {
            nextHole = currentHole;
            break;
        }

        currentHole = currentHole->next;
    }

    // case when the memory is full and there is no Hole

    if (nextHole == NULL && previousHole == NULL)
    {

        // no need to deallocate we will use the effective pointer
        firstHole = object;
        firstHole->next = NULL;
        firstHole->previous = NULL;

        // can be ended
        return;
    }

    // connect the previous hole to the deallocated object(which is a hole now)
    if (previousHole != NULL)
    {
        
        previousHole->next = object;
        object->previous = previousHole;

        // test if the holes can be merged
        if (previousHole->startAddress + previousHole->size == object->startAddress)
        {

            object->size += previousHole->size;
            object->startAddress = previousHole->startAddress;
            object->previous = previousHole->previous;
            //!!!! this should be updated too
            if(previousHole->previous != NULL)
                previousHole->previous->next = object;

            // careful to update the firstHole
            if (previousHole == firstHole)
                firstHole = object;

            // no need to keep it anymore
            free(previousHole);
        }
    }

    // connect the next hole to the deallocated object(which is a hole now)
    if (nextHole != NULL)
    {

        object->next = nextHole;
        nextHole->previous = object;

        // test if the holes can be merged

        if (object->startAddress + object->size == nextHole->startAddress)
        {

            object->size += nextHole->size;
            object->next = nextHole->next;
            //!!!! this should be updated too
            if(nextHole->next != NULL)
                nextHole->next->previous = object;

            // careful to update the firstHole
            if (nextHole == firstHole)
                firstHole = object;
            // no need to keep it anymore
            free(nextHole);
        }
    }

    // case when the nextHole is the firstHole
    while (firstHole->previous != NULL)
    {
        firstHole = firstHole->previous;
    }
}

struct Block *AllocMemory(struct Block *currentHole, size_t processSize)
{

    // create a new allocated block
    struct Block *newObject = (struct Block *)malloc(sizeof(struct Block));
    newObject->size = processSize;
    newObject->startAddress = currentHole->startAddress;
    newObject->next = NULL;
    newObject->previous = NULL;

    // needed blocks
    struct Block *previousHole = currentHole->previous;
    struct Block *nextHole = currentHole->next;
    struct Block *previousObject = NULL;
    struct Block *nextObject = NULL;

    // modify or delete current Hole

    // the hole dissapears when it has exactly the same size as the requested process size
    if (currentHole->size == processSize)
    {

        // must test if the holes exists, otherwise we will just delete the hole

        if (previousHole != NULL)
        {
            previousHole->next = nextHole;
        }
        else
        {
            // if there is  no previous hole then the first hole becomes the next
            firstHole = nextHole;
        }

        
        if (nextHole != NULL)
            nextHole->previous = previousHole;

        free(currentHole);
    }
    // the hole shrinks when the requested process size is less than the hole size
    else
    {

        currentHole->size -= processSize;
        currentHole->startAddress += processSize;
    }

    // test if there are no objects allocated
    if (firstObject == NULL)
    {

        firstObject = newObject;
        return newObject;
    }

    // using this pointer we will iterate through the object list
    struct Block *currentObject = firstObject;

    // find the allocated objects that are between the current OBJECT
    while (currentObject != NULL)
    {
        //!!!
        if (currentObject->startAddress < newObject->startAddress)
        {

            previousObject = currentObject;
            nextObject = currentObject->next;
        }
        else
        {
            // when the first Object that is at a higher address than the hole is found the loop ends and the nextObject is updated
            nextObject = currentObject;
            break;
        }

        currentObject = currentObject->next;
    }

    // place the new object between the found objects (if they exist)

    if (previousObject != NULL)
    {

        previousObject->next = newObject;
        newObject->previous = previousObject;
    }
    else
    {
        // if there is no previous object that means the new object will become the firstObject
        firstObject = newObject;
    }

    if (nextObject != NULL)
    {

        newObject->next = nextObject;
        nextObject->previous = newObject;
    }

    return newObject;
}

//------------------------------------------------------------------------//
