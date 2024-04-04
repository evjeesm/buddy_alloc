#include "buddy_alloc.h"

#include <assert.h>
#include <stdio.h>


typedef enum bd_region_state_t
{
    BD_REGION_FREE = 0,
    BD_REGION_USED
}
bd_region_state_t;


typedef struct bd_header_t
{
    uint8_t state;
    uint8_t reserved[3];
    uint32_t size;
}
bd_header_t;


struct bd_region_t
{
    bd_header_t header;
    char memory[];
};


static size_t align_to_minimal_region(const size_t size);
static size_t ceiled_pow2(const size_t size);
static size_t ul_log2(size_t size);
static void divide_region(bd_region_t *const region);
static bool try_coalesce(bd_region_t *const region);
static void divide_all(bd_region_t *head);


void bd_place(bd_allocator_t *allocator, size_t depth, void *memory)
{
    allocator->arena_size = MINIMAL_REGION_SIZE << depth;
    allocator->head = memory;
    allocator->head->header = (bd_header_t) {
        .size = allocator->arena_size,
    };
}


void* bd_alloc(const bd_allocator_t *const allocator, const size_t req_size)
{
    assert(allocator);
    bd_region_t *region = allocator->head;

    const size_t aligned_size = align_to_minimal_region(req_size + sizeof(bd_header_t));
    assert(aligned_size > req_size);

    const char *end = (char*) region + allocator->arena_size;

    while ((char*) region < end)
    {
        const size_t offset = (char*)region - (char*)allocator->head;
        if (offset % aligned_size != 0 /* missaligned*/
            || BD_REGION_USED == region->header.state) /* or used */
        {
            /* skip block */
            region = (bd_region_t*)((char*) region + region->header.size);
            continue;
        }

        if (aligned_size > region->header.size) /* smaller block */
        {
            /* try coalesce */
            if (!try_coalesce(region))
            {
                /* skip block */
                region = (bd_region_t*)((char*) region + region->header.size);
            }
            continue;
        }

        if (aligned_size < region->header.size) /* greater block */
        {
            divide_region(region);
            continue;
        }

        region->header.state = BD_REGION_USED;
        return region->memory;
    }

    return NULL; /* required region not found */
}


void bd_free(const bd_allocator_t *const allocator, void *const ptr)
{
    assert(allocator);
    assert(ptr);
    assert(ptr < (void*)((char*)allocator->head + allocator->arena_size));

    bd_region_t *region = (bd_region_t*)((char*)ptr - offsetof(bd_region_t, memory));
    assert((region->header.state == BD_REGION_USED) && "double free.");
    region->header.state = BD_REGION_FREE;
}


void bd_allocd_count(const bd_allocator_t *const allocator, const size_t depth, size_t count_arr[const depth], size_t *const total_blocks, size_t *const total_memory)
{
    assert(allocator);
    assert(depth);
    assert(count_arr);
    assert(total_blocks);
    assert(total_memory);

    const bd_region_t *region = allocator->head;
    const char *end = (char*)region + allocator->arena_size;

    while ((char*)region < end)
    {
        if (BD_REGION_USED == region->header.state)
        {
            size_t block_depth = ul_log2(region->header.size);
            ++count_arr[block_depth];
            ++(*total_blocks);
            *total_memory += region->header.size;
        }

        region = (bd_region_t*) ((char*)region + region->header.size);
    }
}


static size_t align_to_minimal_region(const size_t size)
{
    size_t aligned_to_min_region = ((size + MINIMAL_REGION_SIZE - 1)
            / MINIMAL_REGION_SIZE * MINIMAL_REGION_SIZE);
    return ceiled_pow2(aligned_to_min_region);
}


static size_t ceiled_pow2(size_t size)
{
    size_t low = 0;
    size_t test = size;
    while (test > 1)
    {
        ++low;
        test >>= 1;
    }

    size_t ceiled = 1ul << low;
    return (0 == (size ^ ceiled)) ? ceiled : ceiled << 1;
}


static size_t ul_log2(size_t size)
{
    size_t count = 0;
    while (size > 1)
    {
        size >>= 1;
        ++count;
    }
    return count;
}


static void divide_region(bd_region_t *const region)
{
    assert(region->header.size > 0);
    assert(BD_REGION_FREE == region->header.state);

    const uint32_t new_region_size = region->header.size >> 1;
    region->header.size = new_region_size;

    bd_region_t *next = (bd_region_t*) ((char*)region + region->header.size);

    next->header = (bd_header_t){
        .state = BD_REGION_FREE,
        .size = new_region_size,
    };
}


static bool try_coalesce(bd_region_t *const region)
{
    bd_region_t *next = (bd_region_t*) ((char*)region + region->header.size);

    assert(BD_REGION_FREE == region->header.state);
    assert(region->header.size > 0);
    assert(next->header.size > 0);
    assert(next->header.size <= region->header.size);

    if (BD_REGION_USED == next->header.state)
    {
        return false;
    }

    /* attempt merging next blocks up to the size of a region */
    while (next->header.size < region->header.size)
    {
        if (!try_coalesce(next))
        {
            return false;
        }
    }

    assert(next->header.size == region->header.size);
    
    /* next aligned with region */
    region->header.size <<= 1;
    return true;
}


static void divide_all(bd_region_t *head)
{
    const uint32_t new_region_size = head->header.size >> 1;
    if (new_region_size < MINIMAL_REGION_SIZE) return;

    head->header.size = new_region_size;

    bd_region_t *next = (bd_region_t*) ((char*)head + head->header.size);
    next->header.size = new_region_size;

    divide_all(head);
    divide_all(next);
}
