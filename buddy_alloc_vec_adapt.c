#include "buddy_alloc_vec_adapt.h"
#include "buddy_alloc.h"
#include "bd_region.h"

/*
* Preparing global arena storage
*/
BD_STATIC_ARENA(g_arena, BD_ARENA_SIZE);


void *vector_alloc(const size_t alloc_size)
{
    return bd_alloc(&g_arena.allocator, alloc_size);
}


void *vector_realloc(void *ptr, const size_t alloc_size)
{
    return bd_realloc(&g_arena.allocator, ptr, alloc_size);
}


void vector_free(void *ptr)
{
    bd_free(&g_arena.allocator, ptr);
}
