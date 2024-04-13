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

#define BD_STATIC_ARENA(name, ARENA_SZ) \
    struct { \
        bd_allocator_t allocator; \
        union { \
            char arena[ARENA_SZ]; \
            bd_region_t head; \
        }; \
    } \
    static name = { \
        .allocator = { \
            .arena_size = ARENA_SZ, \
            .head = &name.head \
        }, \
        .head = { \
            .header = { \
                .size = ARENA_SZ \
            } \
        } \
    }

/*
* Requires twice as match memory to fill a whole arena
* with smallest blocks with total usable memory equal to `req_size`.
*/
size_t bd_worst_case_alloc_size(const size_t req_size);


/*
* Calculates amount of memory for the allocation 
* that guaranties to fit `block_count` blocks of `block_size`.
*/
size_t bd_alloc_size_for(const size_t block_size, const size_t block_count);


/*
* Create allocator using external memory pool.
* Fits allocator into provided memory buffer.
* The size better be equal to power of 2 in ideal
* and to be multiple of MINIMAL_REGION_SIZE in order to minimize memory waste.
*
* Note: allocator does not occupy space inside memory buffer.
*/
void bd_place(bd_allocator_t *const allocator, const size_t size, char memory[const size]);


/*
* Allocate block of required size.
*/
void *bd_alloc(const bd_allocator_t *const allocator, const size_t req_size);


/*
* Reallocate block to required size.
*/
void *bd_realloc(const bd_allocator_t *const allocator, void *const ptr, const size_t req_size);


/*
* Free previously allocated block.
*/
void bd_free(const bd_allocator_t *const allocator, void *const ptr);


/*
* Count allocated blocks
*/
void bd_allocd_count(const bd_allocator_t *const allocator, const size_t depth, size_t count_arr[const depth], size_t *const total_blocks, size_t *const total_memory);


#endif/*_BUDDY_ALLOC_H_*/
