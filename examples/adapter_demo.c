#include "buddy_alloc.h"
#include "vector.h"

/* you can set you desired size via macro */
#define BD_ARENA_SIZE 4*1024*1024// 4MB
/* macro controls were implementation is going to be defined */
#define BD_STATIC_ARENA_IMPLEMENTATION
#include "bd_static_arena.h"

int main(void)
{
    vector_t *vec = vector_create (
        .element_size=sizeof(int),
        .alloc_opts = alloc_opts (
            .size = sizeof(bd_allocator_t),
            .data = bd_get_static_allocator(),
        ),
    );

    vector_set(vec, 0, TMP_REF(int, 100));

    vector_destroy(vec);

    return 0;
}
