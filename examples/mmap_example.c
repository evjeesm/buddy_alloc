#include "buddy_alloc.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>

bd_allocator_t *make_mmap_allocator(size_t size)
{
    bd_allocator_t *arena = (bd_allocator_t*) mmap(NULL,
        size + sizeof(bd_allocator_t),
        PROT_READ|PROT_WRITE,
        MAP_ANONYMOUS|MAP_PRIVATE,
        -1,
        0);

    if (MAP_FAILED == arena) return NULL;

    bd_place(arena, size, (char*)(arena + 1));
    return arena;
}

#define BUFF_LEN 256

char *handle_input(const bd_allocator_t *allocator)
{
    char *result = bd_alloc(allocator, 8);
    char *buff = bd_alloc(allocator, BUFF_LEN);
    size_t res_size = 0;
    size_t bytes = 0;

    while (1)
    {
        bytes = read(STDIN_FILENO, buff, BUFF_LEN);
        if (buff[0] == 'Q') return result;

        char *ptr = bd_realloc(allocator, result, res_size + bytes);
        if (!ptr) return result;
        result = ptr;
        memcpy(result + res_size, buff, bytes);
        res_size += bytes;
    }

    bd_free(allocator, buff);
    return result;
}

void print(const char *str)
{
    while (*str)
    {
        write(STDOUT_FILENO, str++, 1);
    }
}

void free_allocator(bd_allocator_t *allocator)
{
    munmap(allocator, sizeof(bd_allocator_t) + allocator->arena_size);
}

int main(void)
{
    bd_allocator_t *allocator = make_mmap_allocator(4096);

    char *result = handle_input(allocator);
    print(result);

    free_allocator(allocator);

    return 0;
}

