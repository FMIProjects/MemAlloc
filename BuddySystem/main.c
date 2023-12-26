#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "include/buddy_api.h"

size_t maxMemorySize;
size_t minimumSize;
size_t maximumSize;

int main()
{

    if(buddyInit(MAINBLOCKSIZE, MINIMUMSIZE , MAXIMUMSIZE )){
        perror("Invalid allocator parameters\n\a");
        exit(EXIT_FAILURE);
    }
    printf("Good test\n");

    exit(0);
}