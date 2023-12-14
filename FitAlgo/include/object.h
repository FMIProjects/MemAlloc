#ifndef _MA_OBJECT_H
#define _MA_OBJECT_H

struct Object
{
    int size;
    int startAddress;
    struct Object *next;
    struct Object *previous;
};

#endif