#include <check.h>
#include <stdlib.h>

#include "buddy_alloc.h"

#define DEPTH 8ul
#define MAX_BLOCKS 256
#define MAX_ITERATIONS 2048

static char memory[MINIMAL_REGION_SIZE << DEPTH];
static bd_allocator_t allocator;

static void setup_empty(void)
{
    bd_place(&allocator, MINIMAL_REGION_SIZE << DEPTH, memory);
    ck_assert_uint_eq(allocator.arena_size, MINIMAL_REGION_SIZE << DEPTH);
}

static void teardown(void)
{

}


START_TEST (test_bd_minimal_size)
{
    const size_t req_size = 8;
    for (size_t i = 0; i < MAX_BLOCKS; ++i)
    {
        void *block = bd_alloc(&allocator, req_size);
        memset(block, 0xaa, req_size);
    }

    void *other = bd_alloc(&allocator, 8);
    ck_assert_msg(!other,
        "allocation did not failed when no free blocks left");
}
END_TEST


START_TEST (test_bd_alternate_size)
{
    void *blocks[MAX_BLOCKS];
    size_t allocd = 0;

    for (size_t i = 0; i < MAX_ITERATIONS; ++i)
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
            bd_free(&allocator, blocks[0]);
            for (size_t i = 1; i < allocd; ++i)
            {
                blocks[i - 1] = blocks[i];
            }
            --allocd;
        }
    }
}
END_TEST


START_TEST (test_bd_random_size)
{
    void* blocks[MAX_BLOCKS];

    const size_t max_size = 128;
    const size_t min_size = 8;
    size_t allocd = 0;
    char *block;

    srand(100);
    for (int i = 0; i < MAX_ITERATIONS; ++i)
    {
        size_t size = rand() % (max_size + 1 - min_size) + min_size;
        block = bd_alloc(&allocator, size);
        if (block) /* register allocated block */
        {
            blocks[allocd++] = block;
            memset(block, i, size);
            memcpy(block, &i, sizeof(int));
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
}
END_TEST


START_TEST (test_bd_worst_case_alloc_size)
{
    const size_t req_size = 100;
    const size_t expected_alloc_size = 256;
    size_t alloc_size = bd_worst_case_alloc_size(req_size);
    ck_assert_uint_eq(expected_alloc_size, alloc_size);
}
END_TEST


START_TEST (test_bd_allocd_count)
{
    bd_alloc(&allocator, 8);
    bd_alloc(&allocator, 16);
    bd_alloc(&allocator, 32);
    size_t count[3] = {};
    size_t total_blocks = 0;
    size_t total_memory = 0;

    bd_allocd_count(&allocator, 2, count, &total_blocks, &total_memory);
    ck_assert_uint_eq(count[0], 1);
    ck_assert_uint_eq(count[1], 1);
    ck_assert_uint_eq(count[2], 1);

    ck_assert_uint_eq(total_blocks, 3);
    ck_assert_uint_eq(total_memory, 112);
}
END_TEST


Suite *vector_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Buddy Alloc");
    
    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_checked_fixture(tc_core, setup_empty, teardown);
    tcase_add_test(tc_core, test_bd_minimal_size);
    tcase_add_test(tc_core, test_bd_alternate_size);
    tcase_add_test(tc_core, test_bd_random_size);
    tcase_add_test(tc_core, test_bd_worst_case_alloc_size);
    tcase_add_test(tc_core, test_bd_allocd_count);

    suite_add_tcase(s, tc_core);

    return s;
}


int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = vector_suite();
    sr = srunner_create(s);

    /* srunner_set_fork_status(sr, CK_NOFORK); */
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

