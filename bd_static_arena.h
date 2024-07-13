#ifndef _BD_STATIC_ARENA_H_
#define _BD_STATIC_ARENA_H_

#include "buddy_alloc.h"
#include "bd_region.h"

/*
* Can be used in conjunction with typedef
*/
#define DEFINE_BD_ARENA(ARENA_SZ) struct { \
        bd_allocator_t allocator; \
        union { \
            char arena[ARENA_SZ]; \
            bd_region_t head; \
        }; \
    } \

/*
* Initialize arena structure.
* Add implicit cast if you want assign to already declared variable (reinitialize).
*/
#define INIT_BD_ARENA(ARENA_SZ, name) { \
        .allocator = { \
            .arena_size = ARENA_SZ, \
            .head = &name.head \
        }, \
        .head = { \
            .header = { \
                .size = ARENA_SZ \
            } \
        } \
    }

/*
* For creating local arena on the stack
*/
#define BD_ARENA(name, ARENA_SZ) \
    DEFINE_BD_ARENA(ARENA_SZ) name = INIT_BD_ARENA(ARENA_SZ, name)

/*
* For creating static arena on data segment
*/
#define BD_STATIC_ARENA(name, ARENA_SZ) \
    static BD_ARENA(name, ARENA_SZ)

/*
* Forward declaration of the allocator getter.
*/
bd_allocator_t *bd_get_static_allocator(void);

/*
* STB style implementation
*/
#ifdef BD_STATIC_ARENA_IMPLEMENTATION
    #ifndef BD_ARENA_SIZE
        #define BD_ARENA_SIZE 16 * 1024 // 16K
    #endif/*BD_ARENA_SIZE*/

BD_STATIC_ARENA(g_arena, BD_ARENA_SIZE);

bd_allocator_t *bd_get_static_allocator(void)
{
    return &g_arena.allocator;
}

#endif/*BD_STATIC_ARENA_IMPLEMENTATION*/

#endif/*_BD_STATIC_ARENA_H_*/
