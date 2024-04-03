#include "slab_front.h"

#include <string.h>

#define SLABS 1024

int main(void)
{
    void* ptrs[SLABS];

    ptrs[0] = slab_alloc((1ul << 20) * 16 - 16);
    slab_report();
    slab_free(ptrs[0]);

    for (int i = 0; i < SLABS; ++i)
    {
        size_t size = 4ul << ((i % 5) + 1);
        void *p = slab_alloc(size);
        memset(p, i, size);
        ptrs[i] = p;
    }

    slab_report();

    for (int i = 0; i < SLABS/2; ++i)
    {
        slab_free(ptrs[i]);
    }
    
    slab_report();

    for (int i = 0; i < SLABS/2; ++i)
    {
        size_t size = 8ul << ((i % 5) + 2);
        void *p = slab_alloc(size);
        memset(p, i, size);
        ptrs[i] = p;
    }

    slab_report();

    return 0;
}


