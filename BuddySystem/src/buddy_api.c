#include "../include/buddy_api.h"
#include "../include/colors.h"
#include "../include/raf_params.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <limits.h>
#include <sys/types.h>
#include <time.h>

//--------------------------- Declarations --------------------------------//

extern size_t maxMemorySize;
extern size_t minimumSize;
extern size_t maximumSize;

extern struct BuddyBlock *firstHole;
extern struct BuddyBlock *firstObject;

extern pthread_mutex_t mutex;
extern pthread_cond_t cond;

int signalFlag = 0;
int randomAllocFreeFinished = 0;

// number of allocations
int allocNumber = 0;
// number of frees
int freeNumber = 0;

// used for measuring CPU time
clock_t start;
clock_t end;

//-------------------- Buddy Specific Methods ------------------------//

int BuddyInit(size_t size, size_t low, size_t high)
{

    // if the sizes are not of power of 2 return -1
    if (!(isPowerOfTwo(size) && isPowerOfTwo(low) && isPowerOfTwo(high)))
        return -1;

    // initialize the sizes
    maxMemorySize = size;
    minimumSize = low;
    maximumSize = high;

    // initialize the first hole
    firstHole = (struct BuddyBlock *)malloc(sizeof(struct BuddyBlock));

    firstHole->next = NULL;
    firstHole->previous = NULL;
    firstHole->size = maxMemorySize;
    firstHole->startAddress = 0;
    firstHole->order = log2((double)size / low);

    return 0;
}

struct BuddyBlock *BuddyAlloc(size_t size)
{

    if (size > maximumSize)
        return NULL;

    int currentOrder = calculateOrder(size, minimumSize);

    // by using this pointer we will iterate through the hole blocks
    struct BuddyBlock *currentHole = firstHole;

    // first of all we shall search if there is a suitable block of the same order as the requested size
    while (currentHole != NULL)
    {

        if (currentHole->order == currentOrder)
            break;

        currentHole = currentHole->next;
    }

    // case when there is no suitable hole
    // we need to find the last hole that has the minimum order that is bigger than the order of the current object
    if (currentHole == NULL)
    {

        struct BuddyBlock *chosenHole = NULL;

        // we reuse the currentHole pointer to iterate through the holes
        currentHole = firstHole;
        int minOrder = INT_MAX;

        while (currentHole != NULL)
        {

            // look for the last hole with the minimum order that is bigger than the order of the current object
            if (minOrder >= currentHole->order && currentHole->order > currentOrder)
            {

                minOrder = currentHole->order;
                chosenHole = currentHole;
            }

            currentHole = currentHole->next;
        }

        // if the memory is overloaded and cannot have a block of the given size return null
        if (chosenHole == NULL)
            return NULL;

        // if we have found a Hole we need to start split it into partitons with lower orders till we get one with the order equal to the current order

        currentHole = partitionHole(chosenHole, currentOrder);
    }

    // case when there is a suitable hole

    // connect the next and previous holes if they exist
    struct BuddyBlock *nextHole = currentHole->next;
    struct BuddyBlock *previousHole = currentHole->previous;

    if (previousHole != NULL)
        previousHole->next = nextHole;

    if (nextHole != NULL)
    {
        nextHole->previous = previousHole;
    }

    // update the firstHole if necessary
    if (currentHole == firstHole)
        firstHole = nextHole;

    // sanitise the currentHole, it will become an Object

    currentHole->previous = NULL;
    currentHole->next = NULL;
    currentHole->size = size;

    // case when there is no first object
    if (firstObject == NULL)
    {
        firstObject = currentHole;
        return firstObject;
    }

    // case when the objects need to be connected to other objects

    struct BuddyBlock *nextObject = NULL;
    struct BuddyBlock *previousObject = NULL;
    struct BuddyBlock *currentObject = firstObject;

    // we will find the suitable objects by comparing strat addresses

    while (currentObject != NULL)
    {

        if (currentObject->startAddress < currentHole->startAddress)
        {

            previousObject = currentObject;
            nextObject = currentObject->next;
        }
        else
        {

            nextObject = currentObject;
            break;
        }

        currentObject = currentObject->next;
    }

    // connect the allocated object ( currentHole ) to the found objects ifn they exist
    if (previousObject != NULL)
    {

        previousObject->next = currentHole;
        currentHole->previous = previousObject;
    }

    if (nextObject != NULL)
    {

        currentHole->next = nextObject;
        nextObject->previous = currentHole;
    }

    return currentHole;
}

void FreeBuddyMemory(struct BuddyBlock *object)
{

    // get the next and previous objects and connect them if they exist

    struct BuddyBlock *previousObject = object->previous;
    struct BuddyBlock *nextObject = object->next;

    if (previousObject != NULL)
        previousObject->next = nextObject;

    if (nextObject != NULL)
        nextObject->previous = previousObject;

    // case when the object to deallocate is the first object
    if (object == firstObject)
        firstObject = nextObject;

    // it is safe to sanitize the object since it will become a hole
    object->next = NULL;
    object->previous = NULL;
    // the size becomes the full size of the block
    object->size = (1 << object->order) * minimumSize;

    // find the 2 holes between the object to deallocate

    struct BuddyBlock *previousHole = NULL;
    struct BuddyBlock *nextHole = NULL;
    struct BuddyBlock *currentHole = firstHole;

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

    if (previousHole != NULL)
    {
        object->previous = previousHole;
        previousHole->next = object;
    }
    // case when there is no previous hole
    else
        firstHole = object;

    if (nextHole != NULL)
    {
        nextHole->previous = object;
        object->next = nextHole;
    }

    // now it is time to see if the holes can be merged

    MergeHoles(object);
}

struct BuddyBlock *partitionHole(struct BuddyBlock *chosenHole, int stopOrder)
{

    if (chosenHole == NULL || stopOrder >= chosenHole->order)
        return NULL;

    struct BuddyBlock *previousHole = chosenHole->previous;
    struct BuddyBlock *nextHole = chosenHole->next;

    struct BuddyBlock *newHole = NULL;
    struct BuddyBlock *currentHole = chosenHole;

    while (currentHole->order != stopOrder)
    {

        // create a newHole
        newHole = (struct BuddyBlock *)malloc(sizeof(struct BuddyBlock));
        newHole->startAddress = currentHole->startAddress;
        newHole->size = currentHole->size >> 1;
        newHole->next = currentHole;
        newHole->previous = NULL;
        newHole->order = currentHole->order - 1;

        // split the current Hole into half
        currentHole->size = currentHole->size >> 1;
        currentHole->previous = newHole;
        currentHole->order -= 1;
        currentHole->startAddress += currentHole->size;

        currentHole = newHole;
    }

    // connect the new Hole to the previous hole of the now split chosen hole
    newHole->previous = previousHole;

    // also connect the previous Hole to the newHOle
    if (previousHole != NULL)
        previousHole->next = newHole;

    // also get back the firstHole

    while (firstHole->previous != NULL)
        firstHole = firstHole->previous;

    return newHole;
}

struct BuddyBlock *FindBuddy(struct BuddyBlock *hole)
{

    if (hole == NULL)
        return NULL;

    // find the address of the buddy
    size_t buddyAddress = FindBuddyAddress(hole);

    struct BuddyBlock *previousHole = hole->previous;
    struct BuddyBlock *nextHole = hole->next;

    // return the buddy if it is a hole of the same order as the given hole
    if (previousHole != NULL && previousHole->order == hole->order && previousHole->startAddress == buddyAddress)
        return previousHole;

    if (nextHole != NULL && nextHole->order == hole->order && nextHole->startAddress == buddyAddress)
        return nextHole;

    return NULL;
}

struct BuddyBlock *MergeHoles(struct BuddyBlock *hole)
{

    struct BuddyBlock *buddyBlock = FindBuddy(hole);

    // if there is no buddy just return the hole
    if (buddyBlock == NULL)
        return hole;

    // case when the buddy is the previous hole of the given hole
    if (hole->previous == buddyBlock)
    {

        // make it so that the hole has the buddy on the right
        return MergeHoles(buddyBlock);
    }

    // the current hole merges with its buddy

    // the nextHole will be the buddy's next hole
    struct BuddyBlock *nextHole = buddyBlock->next;

    hole->size += buddyBlock->size;
    ++hole->order;
    hole->next = nextHole;

    if (nextHole != NULL)
        nextHole->previous = hole;

    // the buddy can be freed since it was merged
    free(buddyBlock);

    // continue merging till there is nothing to merge
    return MergeHoles(hole);
}

void BuddyReset()
{

    // deallocate all objects
    while (firstObject != NULL)
    {
        FreeBuddyMemory(firstObject);
    }
}

void BuddyDestroy()
{
    // reset all memory
    BuddyReset();
    // deallocate the whole memory
    free(firstHole);
    // reset the used variables
    firstHole = NULL;
    maxMemorySize = 0;
    minimumSize = 0;
    maximumSize = 0;
}

size_t FindBuddyAddress(struct BuddyBlock *block)
{

    if (block == NULL)
    {
        return (size_t)(0 - 1);
    }

    // get the block size
    size_t blockSize = (1 << block->order) * minimumSize;

    // find the address of the buddy
    return blockSize ^ block->startAddress;
}

int isPowerOfTwo(size_t n)
{
    return (n > 0) && ((n & (n - 1)) == 0);
}

int calculateOrder(size_t size, size_t low)
{
    int order = 0;

    while ((1 << order) * low < size)
        ++order;

    return order;
}

//--------------------------- Utils Methods --------------------------------//

void Menu()
{
    system("clear");
    printf(RED " ____            _     _        _____           _            \n");
    printf("|  _ \\          | |   | |      / ____|         | |           \n");
    printf("| |_) |_   _  __| | __| |_   _| (___  _   _ ___| |_ ___ _ __ ___  \n");
    printf("|  _ <| | | |/ _` |/ _` | | | |\\___ \\| | | / __| __/ _ \\ '_ ` _ \\ \n");
    printf("| |_) | |_| | (_| | (_| | |_| |____) | |_| \\__ \\ ||  __/ | | | | |\n");
    printf("|____/ \\__,_|\\__,_|\\__,_|\\__, |_____/ \\__, |___/\\__\\___|_| |_| |_|\n");
    printf("                        __/ |        __/ |                      \n");
    printf("                       |___/        |___/                       \n" CRESET);
    char input[10];
    printf("==================================================================\n");
    printf("Options:\n");
    printf("==================================================================\n");
    printf(BLU "1 - Start\n" CRESET);
    printf("==================================================================\n");
    printf("Input: ");
    scanf("%s", input);
    int number = atoi(input);
    if (number > 1)
        perror("Wrong Input!"), exit(1);
}

void PrintBlock(struct BuddyBlock *block)
{

    struct BuddyBlock *currentBlock = block;

    int blockNumber = 1;

    if (block == firstObject)
    {

        printf("Objects\n");

        while (currentBlock != NULL)
        {
            size_t wholeBlockSize = (size_t)(1 << currentBlock->order) * minimumSize;

            printf("-----------------------------\n");
            printf("Block number: %d \n", blockNumber);
            printf("Start address of block: %zu \n", currentBlock->startAddress);
            printf("Block order: %d \n", currentBlock->order);
            printf("Allocated size: %zu \n", currentBlock->size);
            printf("Whole block size: %zu \n", wholeBlockSize);
            printf("Internal fragmentation: %zu \n", wholeBlockSize - currentBlock->size);
            printf("-----------------------------\n");

            currentBlock = currentBlock->next;
            ++blockNumber;
        }
    }

    if (block == firstHole)
    {

        printf("Holes\n");
        while (currentBlock != NULL)
        {

            printf("-----------------------------\n");
            printf("Block number: %d \n", blockNumber);
            printf("Start address of block: %zu \n", currentBlock->startAddress);
            printf("Block order: %d \n", currentBlock->order);
            printf("Hole size: %zu \n", currentBlock->size);
            printf("-----------------------------\n");

            currentBlock = currentBlock->next;
            ++blockNumber;
        }
    }
}

int ValidateBlocks()
{
    size_t summedSizes = 0;

    struct BuddyBlock *currentBlock = firstObject;

    while (currentBlock != NULL)
    {
        summedSizes += (1 << currentBlock->order) * minimumSize;
        currentBlock = currentBlock->next;
    }

    currentBlock = firstHole;

    while (currentBlock != NULL)
    {
        summedSizes += currentBlock->size;
        currentBlock = currentBlock->next;
    }

    return (summedSizes == maxMemorySize);
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
        struct BuddyBlock *currentHole = firstHole;
        struct BuddyBlock *currentBlock = firstObject;
        float externalFragmentation = 0;
        float internalFragmentation = 0;
        int holeNumber = 0;

        pthread_mutex_lock(&mutex);
        while (currentHole != NULL)
        {
            holeNumber++;
            externalFragmentation += currentHole->size;
            currentHole = currentHole->next;
        }

        while (currentBlock != NULL)
        {
            size_t wholeBlockSize = (size_t)(1 << currentBlock->order) * minimumSize;

            internalFragmentation += wholeBlockSize - currentBlock->size;
            currentBlock = currentBlock->next;
        }

        pthread_mutex_unlock(&mutex);

        printf("Memory Statistics:\n");
        printf("=========================================\n");
        printf(BLU "Number of allocations: %d\n", allocNumber);
        printf("Number of deallocations: %d\n", freeNumber);
        printf("Number of holes: %d\n" CRESET, holeNumber);
        printf(RED "External Fragmentation: %.6f KB\n" CRESET, externalFragmentation / 1000);
        printf(RED "Internal Fragmentation: %.6f KB\n" CRESET, internalFragmentation / 1000);
        printf(YEL "CPU Time: %.3f seconds\n" CRESET, elapsed);
        printf("=========================================\n");
        // End of Statistics Logic
    }

    pthread_exit(NULL);
    return NULL;
}

void *RandomAllocFree(void *arg)
{
    start = clock();

    struct RandomAllocFreeParams *params = (struct RandomAllocFreeParams *)arg;

    int option = params->option;
    size_t *array = params->sizes;

    static unsigned int seed = 0;
    srand(time(NULL) + seed);

    size_t afRandom;
    size_t freeIndexRandom;
    size_t allocIndexRandom;
    size_t indexAlloc = 0;

    if (option == 1)
    {
        for (int i = 0; i < AFNUMBER; i++)
        {
            afRandom = (rand() % 2) + 1;
            if (afRandom == 1)
            {
                allocNumber++;
                allocIndexRandom = (rand() % OBJECTNUMBER) + 1;

                pthread_mutex_lock(&mutex);
                BuddyAlloc(array[allocIndexRandom]);
                indexAlloc++;
                pthread_mutex_unlock(&mutex);
            }
            else
            {
                if (indexAlloc != 0)
                {
                    freeNumber++;
                    freeIndexRandom = (rand() % indexAlloc) + 1;

                    struct BuddyBlock *currentObject = firstObject;
                    size_t currentIndex = 0;

                    pthread_mutex_lock(&mutex);
                    while (currentObject != NULL)
                    {
                        currentIndex++;
                        if (currentIndex == freeIndexRandom)
                            break;

                        currentObject = currentObject->next;
                    }

                    if (currentObject != NULL)
                    {
                        FreeBuddyMemory(currentObject);
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