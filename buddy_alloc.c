#include "buddy_alloc.h"
#include "bd_region.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static size_t align_to_minimal_region(const size_t size);
static size_t ceiled_pow2(const size_t size);
static size_t ul_log2(size_t size);
static void divide_region(bd_region_t *const region);
static bool try_coalesce(bd_region_t *const region, const size_t req_size);
static bool coalesce_check(const bd_region_t *start, const size_t req_size);
static void divide_all(bd_region_t *head);


size_t calc_worst_case_depth(const size_t req_size)
{
    size_t worst_case_size = align_to_minimal_region(req_size) * 2;
    return ul_log2(worst_case_size / MINIMAL_REGION_SIZE);
}


bool bd_create(bd_allocator_t *const allocator, const size_t depth)
{
    assert(allocator);

    void *memory = (void*)malloc(MINIMAL_REGION_SIZE << depth);
    if (!memory) return false;

    bd_place(allocator, depth, memory);
    return true;
}


void bd_destroy(bd_allocator_t *const allocator)
{
    assert(allocator);
    assert(allocator->head);

    free(allocator->head);
}


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
            if (!try_coalesce(region, aligned_size))
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


void *bd_realloc(const bd_allocator_t *const allocator, void *const ptr, const size_t req_size)
{
    assert(allocator);
    assert(ptr);
    assert(ptr < (void*)((char*)allocator->head + allocator->arena_size));

    bd_region_t *region = (bd_region_t*)((char*)ptr - offsetof(bd_region_t, memory));
    assert((region->header.state == BD_REGION_USED) && "realloc of freed block");

    const size_t aligned_size = align_to_minimal_region(req_size + sizeof(bd_header_t));
    if (region->header.size >= aligned_size) return ptr;

    /* check if current block can be extended without relocation */
    bd_region_t *next = (bd_region_t*) ((char*)region + region->header.size);
    if (coalesce_check(next, aligned_size - region->header.size))
    {
        region->header.size = aligned_size;
        return ptr;
    }

    void *other = bd_alloc(allocator, req_size);
    if (!other) return NULL;

    const size_t size_to_copy = region->header.size > aligned_size
            ? aligned_size
            : region->header.size;

    memcpy(other, ptr, size_to_copy);
    bd_free(allocator, ptr);

    return other;
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
            size_t block_depth = ul_log2(region->header.size/MINIMAL_REGION_SIZE);
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


static bool coalesce_check(const bd_region_t *start, const size_t aligned_size)
{
    const char *end = (char*) start + aligned_size;

    while ((char*)start < end)
    {
        if (BD_REGION_USED == start->header.state)
        {
            return false;
        }
        start = (bd_region_t*) ((char*)start + start->header.size);
    }

    return true;
}


static bool try_coalesce(bd_region_t *const region, const size_t req_size)
{
    bd_region_t *next = (bd_region_t*) ((char*)region + region->header.size);

    assert(BD_REGION_FREE == region->header.state);
    assert(region->header.size > 0);
    assert(next->header.size > 0);
    assert(next->header.size <= region->header.size);

    if (!coalesce_check(next, req_size - region->header.size))
    {
        return false;
    }

    region->header.size = req_size;
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
