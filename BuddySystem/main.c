#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "include/buddy_api.h"
#include "include/buddyblock.h"

size_t maxMemorySize;
size_t minimumSize;
size_t maximumSize;

struct BuddyBlock* firstHole;
struct BuddyBlock* firstObject;

int main()
{

    if(buddyInit(MAINBLOCKSIZE, MINIMUMSIZE , MAXIMUMSIZE )){

        perror("Invalid allocator parameters\n\a");
        exit(EXIT_FAILURE);
    }
    
    

    exit(0);
}