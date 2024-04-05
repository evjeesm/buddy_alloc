#include "buddy_alloc_vec_adapt.h"
#include "buddy_alloc.h"
#include "bd_region.h"

typedef struct arena_t
{
    bd_allocator_t allocator;
    union {
        char arena[BD_ARENA_SIZE];
        bd_region_t head;
    };
}
arena_t;


static arena_t g_arena = {
    .allocator = {
        .arena_size = BD_ARENA_SIZE,
        .head = &g_arena.head
    },
    .head = {
        .header = {
            .size = BD_ARENA_SIZE
        }
    }
};


void *vector_alloc(const size_t alloc_size)
{
    return bd_alloc(&g_arena.allocator, alloc_size);
}


void *vector_realloc(void *ptr, const size_t alloc_size)
{
    void *other = bd_realloc(&g_arena.allocator, ptr, alloc_size);
    if (!other) return NULL;
    return other;
}


void vector_free(void *ptr)
{
    bd_free(&g_arena.allocator, ptr);
}
