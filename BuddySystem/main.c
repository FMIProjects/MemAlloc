#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "include/buddy_api.h"
#include "include/buddyblock.h"
#include "include/raf_params.h"
#include <math.h>

size_t vSizesToAllocate[OBJECTNUMBER];

size_t maxMemorySize;
size_t minimumSize;
size_t maximumSize;

struct BuddyBlock *firstHole;
struct BuddyBlock *firstObject;

// used for lock/unlock mechanism
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// used for waking up statistics thread waiting for condition variable.
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int main()
{

    if (BuddyInit(MAINBLOCKSIZE, MINIMUMSIZE, MAXIMUMSIZE))
    {
        perror("Invalid allocator parameters\n\a");
        exit(EXIT_FAILURE);
    }
    
    GenerateRandomSizes(vSizesToAllocate);

    GenerateRandomSizes(vSizesToAllocate);

    Menu();

    struct RandomAllocFreeParams params = {
        .sizes = vSizesToAllocate,
        .option = 1};
    
    // create the threads
    pthread_t threadRandomAllocFree, threadStatistics;
    pthread_create(&threadRandomAllocFree, NULL, RandomAllocFree, (void *)&params);


    struct BuddyBlock *blk[100];
    blk[0] = BuddyAlloc(16000);
    FreeBuddyMemory(blk[0]);



    pthread_join(threadRandomAllocFree, NULL);



    BuddyDestroy();
    exit(0);
}