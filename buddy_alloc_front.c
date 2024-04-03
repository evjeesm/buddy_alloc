#include "buddy_alloc_front.h"

#include <stdlib.h>
#include <stdio.h>

static bd_allocator_t g_allocator;

void *slab_alloc(size_t size)
{
    return bd_alloc(&g_allocator, size);
}

void slab_free(void *ptr)
{
    bd_free(&g_allocator, ptr);
}


void slab_report(void)
{

}

