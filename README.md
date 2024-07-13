# Buddy Alloc

Variable size allocator. Manages fixed size memory block.
Memory source for the allocator is up to user. (heap, data, mmap, etc...)
Allocation performed by slicing block in half recursively,
when smaller block requested and vise versa, coalescing free blocks to get a bigger one.

## Complexity
Best case:  `O(LogN), N - amount of blocks of requested size that allocator can fit`
Worst case: `O(M), M - amount of allocated blocks`

## Integration with other projects
- includes `buddy_alloc_vec_adapt`, adapter for [vector](https://github.com/evjeesm/vector.git)

