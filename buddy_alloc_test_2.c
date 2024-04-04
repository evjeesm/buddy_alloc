#include "buddy_alloc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_BLOCKS 256
#define MAX_ITERATIONS 1024*16
#define DEPTH 8ul /* 256 * 16 */

char memory[MINIMAL_REGION_SIZE << DEPTH] = {0};

int test_allocate_all_16(void)
{
    bd_allocator_t allocator;
    bd_place(&allocator, DEPTH, memory);

    for (size_t i = 0; i < MAX_BLOCKS; ++i)
    {
        void *block = bd_alloc(&allocator, 8);
        memset(block, 0xaa, 8);
    }

    void *other = bd_alloc(&allocator, 8);
    if (other) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

int test_allocate_diff(void)
{
    bd_allocator_t allocator;
    void* blocks[MAX_BLOCKS] = {0};
    size_t allocd = 0;

    bd_place(&allocator, DEPTH, memory);

    size_t i = 0;
    for (; i < MAX_ITERATIONS; ++i)
    {
        size_t size = 8ul << (i % 7);
        void *block = bd_alloc(&allocator, size);
        if (block)
        {
            memset(block, 0xaa, size);
            blocks[allocd++] = block;
        }
        else
        {
            break;
        }
    }

    /* free all blocks */
    for (size_t a = 0; a < allocd; ++a)
    {
        bd_free(&allocator, blocks[a]);
    }
    allocd = 0;

    for (; i < MAX_ITERATIONS; ++i)
    {
        size_t size = 8ul << (i % 7);
        void *block = bd_alloc(&allocator, size);
        if (block)
        {
            memset(block, 0xaa, size);
            blocks[allocd++] = block;
        }
        else
        {
            break;
        }
    }

    /* free all blocks */
    for (size_t a = 0; a < allocd; ++a)
    {
        bd_free(&allocator, blocks[a]);
    }
    allocd = 0;

    for (; i < MAX_ITERATIONS; ++i)
    {
        size_t size = 8ul << (i % 7);
        void *block = bd_alloc(&allocator, size);
        if (block)
        {
            memset(block, 0xbc, size);
            blocks[allocd++] = block;
        }
        else
        {
            // bd_free(&allocator, blocks[--allocd]);
            bd_free(&allocator, blocks[0]);
            for (size_t i = 1; i < allocd; ++i)
            {
                blocks[i - 1] = blocks[i];
            }
            --allocd;
        }
    }

    /* free all blocks */
    for (size_t a = 0; a < allocd; ++a)
    {
        bd_free(&allocator, blocks[a]);
    }
    allocd = 0;

    for (size_t i = 0; i < MAX_BLOCKS; ++i)
    {
        void *block = bd_alloc(&allocator, 8);
        memset(block, 0xaa, 8);
    }

    void *other = bd_alloc(&allocator, 8);
    if (other) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

int main(void)
{
    if (EXIT_FAILURE == test_allocate_all_16()) exit(EXIT_FAILURE);
    if (EXIT_FAILURE == test_allocate_diff()) exit(EXIT_FAILURE);

    return 0;
}


