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

    const char *end = (char*) region + allocator->arena_size;

    while ((char*) region < end)
    {
        const size_t offset = (char*)region - (char*)allocator->head;
        if (offset % aligned_size != 0 /* missaligned*/
            || region->header.state == BD_REGION_USED) /* or used */
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

    bd_region_t *region = (bd_region_t*)((char*)ptr - offsetof(bd_region_t, memory));
    assert(region->header.state == BD_REGION_USED && "double free.");
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


void divide_all(bd_region_t *head)
{
    const size_t new_region_size = head->header.size >> 1;
    if (new_region_size < MINIMAL_REGION_SIZE) return;

    head->header.size = new_region_size;

    bd_region_t *next = (bd_region_t*) ((char*)head + head->header.size);
    next->header.size = new_region_size;

    divide_all(head);
    divide_all(next);
}


static size_t align_to_minimal_region(const size_t size)
{
    size_t aligned_to_min_region = ((size + MINIMAL_REGION_SIZE - 1) / MINIMAL_REGION_SIZE * MINIMAL_REGION_SIZE);
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
    const size_t new_region_size = region->header.size >> 1;
    region->header.size = new_region_size;

    bd_region_t *next = (bd_region_t*) ((char*)region + region->header.size);
    next->header.size = new_region_size;
    assert(next->header.size > 0);
}


static bool try_coalesce(bd_region_t *const region)
{
    bd_region_t *next = (bd_region_t*) ((char*)region + region->header.size);

    assert(region->header.size > 0);
    assert(next->header.size > 0);

    if (BD_REGION_USED == next->header.state)
    {
        return false;
    }

    /* attempt merging next blocks up to the size of a region */
    if (next->header.size < region->header.size
        && !try_coalesce(next))
    {
        return false;
    }

    /* next aligned with region */
    region->header.size <<= 1;
    return true;
}

#include <stdlib.h>
#include <time.h>
#define DEPTH 16ul

char memory[MINIMAL_REGION_SIZE << DEPTH];

int main(void)
{
    printf("header size: %zu\n", sizeof(bd_header_t));
    printf("memory size over depth = %zu -> %zu\n", DEPTH, sizeof(memory));

    bd_allocator_t allocator;

    bd_place(&allocator, DEPTH, memory);
    // divide_all(allocator.head);

    char *block;
    size_t max_size = 4000;
    size_t min_size = 16;
    size_t i = 0;
    srand(100);
    while (max_size > min_size)
    {
        do
        {
            size_t size = rand() % (max_size + 1 - min_size) + min_size;
            block = bd_alloc(&allocator, size);
            ++i;
        }
        while (block);
        max_size >>= 1;
    }

    size_t count_arr[DEPTH] = {0};
    size_t total_blocks = 0;
    size_t total_memory = 0;
    bd_allocd_count(&allocator, DEPTH, count_arr, &total_blocks, &total_memory);
    printf("allocd blocks in total: %zu \n", total_blocks);
    printf("allocd memory in total: %zu \n", total_memory);




    return 0;
}
