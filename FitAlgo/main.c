#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include "include/memalloc_api.h"
#include "include/block.h"
#include "include/raf_params.h"

size_t vSizesToAllocate[OBJECTNUMBER];
struct Block *firstHole = NULL;
struct Block *firstObject = NULL;

void *memory;

int algorithm;

// used for lock/unlock mechanism
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// used for waking up statistics thread waiting for condition variable.
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int main()
{
    memory = AllocMainBlock();

    // create the first hole which will be the whole memory
    firstHole = (struct Block *)malloc(sizeof(struct Block));
    firstHole->size = MAINBLOCKSIZE;
    firstHole->startAddress = 0;
    firstHole->previous = NULL;
    firstHole->next = NULL;

    GenerateRandomSizes(vSizesToAllocate);

    algorithm = Menu();

    struct RandomAllocFreeParams params = {
        .sizes = vSizesToAllocate,
        .algorithm = algorithm};

    // create the threads
    pthread_t threadRandomAllocFree, threadStatistics;
    pthread_create(&threadRandomAllocFree, NULL, RandomAllocFree, (void *)&params);
    pthread_create(&threadStatistics, NULL, Statistics, NULL);

    // wait for threads to finish
    pthread_join(threadRandomAllocFree, NULL);
    pthread_join(threadStatistics, NULL);

    free(memory);
    exit(0);
}
