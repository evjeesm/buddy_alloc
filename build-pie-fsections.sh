#!/bin/sh

CC=gcc
CFLAGS='-Wall -Wextra -std=c11 -DNDEBUG -O3 -fpic -ffunction-sections -fdata-sections -I. -Ivector/src'
# LDFLAGS='-L. -L.libs -Lvector/src/.libs'
LDFLAGS='-L.'

$CC $CFLAGS -c adapter_demo.c -o adapter_demo.o
$CC $CFLAGS -c vector/src/vector.c -o vector.o
$CC $CFLAGS -c buddy_alloc.c -o buddy_alloc.o
$CC $CFLAGS -c buddy_alloc_vec_adapt.c -o buddy_alloc_vec_adapt.o
ar -cr liball.a buddy_alloc.o buddy_alloc_vec_adapt.o vector.o adapter_demo.o

$CC $CFLAGS -fpie -o adapter_demo $LDFLAGS -Wl,--whole-archive,--gc-sections -lall -Wl,--no-whole-archive
