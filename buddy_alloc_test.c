#include "buddy_alloc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define DEPTH 16ul
#define TEST_1_BLOCKS 377
#define TEST_2_ITERATIONS 1000000
#define TEST_2_BLOCKS 2000

char memory[MINIMAL_REGION_SIZE << DEPTH];


int test_random_1(void)
{
    bd_allocator_t allocator;
    bd_place(&allocator, DEPTH, memory);

    const size_t expected_blocks_count = TEST_1_BLOCKS;
    void* blocks[TEST_1_BLOCKS];

    const size_t min_size = 16;
    size_t max_size = 4000;
    size_t allocd = 0;
    char *block;

    srand(100);
    while (max_size > min_size)
    {
        do
        {
            size_t size = rand() % (max_size + 1 - min_size) + min_size;
            block = bd_alloc(&allocator, size);
            if (block) /* register allocated block */
            {
                blocks[allocd++] = block;
                memset(block, 0, size);
            }
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

    if (total_blocks != allocd) return EXIT_FAILURE;
    if (total_memory != allocator.arena_size) return EXIT_FAILURE;

    for (size_t i = 0; i < total_blocks; ++i)
    {
        bd_free(&allocator, blocks[i]);
    }

    block = bd_alloc(&allocator, allocator.arena_size - 8);

    if (!block) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}


int test_random_2(void)
{
    bd_allocator_t allocator;
    bd_place(&allocator, DEPTH, memory);

    const size_t expected_blocks_count = TEST_2_BLOCKS;
    void* blocks[TEST_2_BLOCKS];

    const size_t max_size = 2048;
    const size_t min_size = 16;
    size_t allocd = 0;
    char *block;

    srand(100);
    for (int i = 0; i < TEST_2_ITERATIONS; ++i)
    {
        size_t size = rand() % (max_size + 1 - min_size) + min_size;
        block = bd_alloc(&allocator, size);
        if (block) /* register allocated block */
        {
            blocks[allocd++] = block;
            memset(block, i, size);
        }
        else
        {
            bd_free(&allocator, blocks[0]);
            memmove(blocks, &blocks[1], sizeof(void*) * (allocd - 1));
            --allocd;
        }
    }


    return EXIT_SUCCESS;
}


int main(void)
{
    printf("memory size over depth = %zu -> %zu\n", DEPTH, sizeof(memory));

    if (test_random_1()) assert(false);
    if (test_random_2()) assert(false);


    return 0;
}


