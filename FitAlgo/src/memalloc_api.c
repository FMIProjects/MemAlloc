#include "../include/memalloc_api.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

//--------------------------- Declarations --------------------------------//

extern struct Block *firstHole;
extern struct Block *firstObject;
extern void *memory;

extern pthread_mutex_t mutex;
extern pthread_cond_t cond;

int signalFlag = 0;
int randomAllocFreeFinished = 0;

int allocNumber = 0;
int freeNumber = 0;

//used for measuring CPU time
clock_t start;
clock_t end;


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

int ValidateBlocks()
{
    size_t summedSizes = 0;

    struct Block *currentBlock = firstObject;

    while (currentBlock != NULL)
    {
        summedSizes += currentBlock->size;
        currentBlock = currentBlock->next;
    }

    currentBlock = firstHole;

    while (currentBlock != NULL)
    {
        summedSizes += currentBlock->size;
        currentBlock = currentBlock->next;
    }

    return (summedSizes == MAINBLOCKSIZE);
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

void *RandomAllocFree(void *arg)
{
    start = clock();

    struct RandomAllocFreeParams *params = (struct RandomAllocFreeParams *)arg;

    int algo = params->algorithm;
    size_t *array = params->sizes;

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
            allocNumber++;

            allocIndexRandom = (rand() % OBJECTNUMBER) + 1;
            pthread_mutex_lock(&mutex);
            switch (algo)
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
                pthread_mutex_unlock(&mutex);
                return NULL;
            }
            indexAlloc++;
            pthread_mutex_unlock(&mutex);
        }
        else
        {
            if (indexAlloc != 0)
            {
                freeNumber++;
                freeIndexRandom = (rand() % indexAlloc) + 1;
                struct Block *currentObject = firstObject;
                size_t currentIndex = 0;

                pthread_mutex_lock(&mutex);
                // search into the list the first large enough hole
                while (currentObject != NULL)
                {
                    currentIndex++;
                    if (currentIndex == freeIndexRandom)
                        break;

                    currentObject = currentObject->next;
                }

                if (currentObject != NULL)
                {
                    FreeMemory(currentObject);
                    indexAlloc--;
                }
                pthread_mutex_unlock(&mutex);
            }
        }
        if (i % 1000 == 0)
        {
            pthread_mutex_lock(&mutex);
            // Signal the Statistics thread to print
            end = clock();
            signalFlag = 1;
            pthread_cond_signal(&cond);
            pthread_mutex_unlock(&mutex);
        }
    }

    pthread_mutex_lock(&mutex);
    randomAllocFreeFinished = 1;
    // Signal the Statistics thread to check the flag
    // to exit the loop
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
    return NULL;
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
    system("clear");
    printf(RED"    ______ _ _            _             \n");
    printf("   |  ____(_) |     /\\   | |            \n");
    printf("   | |__   _| |_   /  \\  | | __ _  ___  \n");
    printf("   |  __| | | __| / /\\ \\ | |/ _` |/ _ \\ \n");
    printf("   | |    | | |_ / ____ \\| | (_| | (_) |\n");
    printf("   |_|    |_|\\__/_/    \\_\\_|\\__, |\\___/ \n");
    printf("                             __/ |      \n");
    printf("                            |___/       \n"CRESET);
    char input[10];
    printf("=============================================\n");
    printf("Choose a memory allocation algorithm:\n");
    printf("=============================================\n");
    printf(BLU"1 - First Fit\n");
    printf("2 - Next Fit\n");
    printf("3 - Best Fit\n");
    printf("4 - Worst Fit\n" CRESET);
    printf("=============================================\n");
    printf("Algorithm: ");
    scanf("%s", input);
    int algorithm = atoi(input);
    if (algorithm > 4)
        perror("Wrong Input!"), exit(1);
    return algorithm;
}

void *Statistics()
{
    while (1)
    {
        pthread_mutex_lock(&mutex);

        // Statistics Methos waits for a signal from RandomAllocFree Method
        while (!signalFlag && !randomAllocFreeFinished)
        {
            pthread_cond_wait(&cond, &mutex);
        }
        // reset the signal to wait again for another
        signalFlag = 0;
        double elapsed = ((double)(end - start)) / CLOCKS_PER_SEC;
        pthread_mutex_unlock(&mutex);

        // if RandomAllocFree finished we close the statistics thread
        if (randomAllocFreeFinished)
            break;

        // Statistics Logic
        system("clear");
        struct Block *currentHole = firstHole;
        float externalFragmentation = 0;
        int holeNumber = 0;

        while (currentHole != NULL)
        {
            holeNumber++;
            externalFragmentation += currentHole->size;
            currentHole = currentHole->next;
        }

        printf("Memory Statistics:\n");
        printf("=========================================\n");
        printf(BLU"Number of allocations: %d\n", allocNumber);
        printf("Number of deallocations: %d\n", freeNumber);
        printf("Number of holes: %d\n"CRESET, holeNumber);
        printf(RED"External Fragmentation: %.6f KB\n"CRESET, externalFragmentation / 1000);
        printf(YEL"CPU Time: %.3f seconds\n"CRESET, elapsed);
        printf("=========================================\n");
        // End of Statistics Logic
    }

    pthread_exit(NULL);
    return NULL;
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
    if (object == NULL)
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
            if (previousHole->previous != NULL)
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
            if (nextHole->next != NULL)
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
