#ifndef _MA_OBJECT_H
#define _MA_OBJECT_H

#include<stddef.h>

struct Object
{
    size_t size;
    size_t startAddress;
    struct Object *next;
    struct Object *previous;
};

#endif