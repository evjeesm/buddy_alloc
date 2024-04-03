#ifndef _BUDDY_ALLOC_FRONT_H_
#define _BUDDY_ALLOC_FRONT_H_

#include "buddy_alloc.h"

#define SLAB_ARENA_SIZE 20

void* slab_alloc(size_t size);
void slab_free(void* ptr);
void slab_report(void);

#endif/*_BUDDY_ALLOC_FRONT_H_*/
