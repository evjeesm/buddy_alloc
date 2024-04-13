#include "buddy_alloc.h"
#include "bd_region.h"

#include <assert.h>
#include <string.h>

#define ALIGN(x, alignment) (((x) + alignment - 1) / alignment * alignment)

/* aligned size will be large at least as `size` and greater,
 * aligned to power of 2 and multible of MINIMAL_REAGION_SIZE. */
static size_t align_to_minimal_region(const size_t size);

/* returns closest ceiled power of 2 to the size */
static size_t ceiled_pow2(const size_t size);

/* returns closest floored power of 2 to the size */
static size_t floored_pow2(const size_t size);

static size_t ul_log2(size_t size);
static void divide_region(bd_region_t *const region);
static bool coalesce_check(const bd_region_t *start, const size_t req_size);
static void divide_all(bd_region_t *head);


size_t bd_worst_case_alloc_size(const size_t req_size)
{
    return align_to_minimal_region(req_size * 2);
}


size_t bd_alloc_size_for(const size_t block_size, const size_t block_count)
{
    size_t req_alloc_size = align_to_minimal_region((block_size + sizeof(bd_region_t)) * block_count);
    return req_alloc_size;
}


void bd_place(bd_allocator_t *const allocator, const size_t size, char memory[const size])
{
    const size_t blocks_fit = size / MINIMAL_REGION_SIZE;
    size_t ceiled_fit = floored_pow2(blocks_fit);
  
    allocator->arena_size = ceiled_fit * MINIMAL_REGION_SIZE;
    allocator->head = (bd_region_t*)memory;
    allocator->head->header = (bd_header_t) {
        .size = allocator->arena_size,
    };
}


void* bd_alloc(const bd_allocator_t *const allocator, const size_t req_size)
{
    assert(allocator);
    assert(req_size != 0);

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
            if (coalesce_check(region, aligned_size))
            {
                region->header.size = aligned_size;
            }
            else
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
    assert(req_size != 0);

    bd_region_t *region = (bd_region_t*)((char*)ptr - offsetof(bd_region_t, memory));
    assert((region->header.state == BD_REGION_USED) && "realloc of freed block");

    const size_t aligned_size = align_to_minimal_region(req_size + sizeof(bd_header_t));
    if (region->header.size >= aligned_size) return ptr;

    /* check if current block can be extended without relocation */
    if (coalesce_check(region, aligned_size))
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
    size_t aligned_to_min_region = ALIGN(size, MINIMAL_REGION_SIZE);
    return ceiled_pow2(aligned_to_min_region);
}


static size_t ceiled_pow2(const size_t size)
{
    size_t pow2 = floored_pow2(size);
    return (0 == (size ^ pow2)) ? pow2 : pow2 << 1;
}


static size_t floored_pow2(const size_t size)
{
    size_t low = 0;
    size_t test = size;
    while (test > 1)
    {
        ++low;
        test >>= 1;
    }

    return 1ul << low;
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
