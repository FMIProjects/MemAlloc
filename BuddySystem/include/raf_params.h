#ifndef _MA_RAF_PARAMS_H
#define _MA_RAF_PARAMS_H

#include <stddef.h>

struct RandomAllocFreeParams
{
    size_t *sizes;
    int option;
};

#endif