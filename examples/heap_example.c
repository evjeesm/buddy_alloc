#include "buddy_alloc.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

/*
 * Lets allocate arena on the heap.
 */
void make_heap_allocator(bd_allocator_t *allocator, size_t size)
{
    const size_t alloc_size = bd_worst_case_alloc_size(size);
    void *arena = malloc(alloc_size);
    assert(arena);
    bd_place(allocator, alloc_size, arena);
}

/*
 * Allocator can be passed by value.
 */
char* allocate_bunch_of_strs_and_concat(const bd_allocator_t allocator)
{
    char *strs[10];
    for (size_t i = 0; i < 10; ++i)
    {
        strs[i] = (char*) bd_alloc(&allocator, 16);
        sprintf(strs[i], "hello %zu", i);
    }

    char *concat = bd_alloc(&allocator, 10 * 16);
    *concat = '\0'; // empty string

    for (size_t i = 0; i < 10; i += 2)
    {
        strcat(concat, strs[i]);
        strcat(concat, " - ");
    }

    strcat(concat, "\n");

    return concat;
}

int main(void)
{
    /* allocator stores arena size
     * and pointer to begining of the memory */
    bd_allocator_t allocator;

    /* 4k is enough */
    make_heap_allocator(&allocator, 4096);

    /* perform some manipulations */
    printf("concat: %s\n", allocate_bunch_of_strs_and_concat(allocator));

    /* Don't forget to free arena at the end */
    free(allocator.head);

    return 0;
}
