#ifndef _MA_FIRST_FIT_H
#define _MA_FIRST_FIT_H


#include "../block.h"

struct Object* firstFit(size_t size, void* mainMemory , struct Object* objectToBlock,struct Hole* holes);

#endif