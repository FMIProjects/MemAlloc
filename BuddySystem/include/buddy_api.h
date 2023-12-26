#ifndef _MA_BUDDY_API_H
#define _MA_BUDDY_API_H

#endif

#include<stddef.h>

//--------------------------- Defines --------------------------------//

// one MB is 1048576 (2^20) bytes => 128 MB is 134217728 (2^27) bytes 
#define MAINBLOCKSIZE 134217728
// the minimum size that can be allocated is 1024 (2^10) bytes => 1MB/1024B
#define MINIMUMSIZE  1024
// the maximum size that can be allocated is 64 MB => 67108864 (2^26) bytes
#define MAXIMUMSIZE 67108864

//--------------------------- Methods -------------------------------//

// Method used for buddy init
int isPowerOfTwo(size_t n);

// Method used to initialise the size and bounds of the allocator 
int buddyInit(size_t size, size_t low, size_t high);