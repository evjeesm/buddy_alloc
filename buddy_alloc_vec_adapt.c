#include "buddy_alloc.h"

void *vector_alloc(const size_t alloc_size, void *const param)
{
    bd_allocator_t *allocator = param;
    return bd_alloc(allocator, alloc_size);
}


void *vector_realloc(void *ptr, const size_t alloc_size, void *const param)
{
    bd_allocator_t *allocator = param;
    return bd_realloc(allocator, ptr, alloc_size);
}


void vector_free(void *ptr, void *const param)
{
    bd_allocator_t *allocator = param;
    bd_free(allocator, ptr);
}
