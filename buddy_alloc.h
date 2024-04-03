#ifndef _BUDDY_ALLOC_H_
#define _BUDDY_ALLOC_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define MINIMAL_REGION_SIZE 16

typedef struct bd_region_t bd_region_t;

typedef struct
{
    size_t arena_size;
    bd_region_t* head;
}
bd_allocator_t;


/*
* Creates allocator on the heap. Arena size will be calculated from
* MINIMAL_REGION_SIZE in power of depth argument.
*/
bool bd_create(bd_allocator_t *const allocator, const size_t depth);


/*
* Destroy allocator on the heap.
*/
void bd_destroy(bd_allocator_t *const allocator);


/*
* Create allocator using external memory pool.
* Size of the memory region has to be at least MINIMAL_REGION_SIZE ^ depth large.
*/
void bd_place(bd_allocator_t *const allocator, const size_t depth, void *memory);


/*
* Allocate block of required size.
*/
void *bd_alloc(const bd_allocator_t *const allocator, const size_t req_size);


/*
* Free previously allocated block.
*/
void bd_free(const bd_allocator_t *const allocator, void *const ptr);


/*
* Count allocated blocks
*/
void bd_allocd_count(const bd_allocator_t *const allocator, const size_t depth, size_t count_arr[const depth], size_t *const total_blocks, size_t *const total_memory);


#endif/*_BUDDY_ALLOC_H_*/