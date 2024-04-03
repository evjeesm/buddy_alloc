#!/bin/sh

gcc -std=c11 -g -O0 -fpic -c buddy_alloc.c
gcc -std=c11 -g -O0 -fpic -c buddy_alloc_front.c
gcc -std=c11 -g -O0 -fpic -c buddy_alloc_test.c
gcc -std=c11 -g -O0 -fpic -shared buddy_alloc.o buddy_alloc_front.o -o libbuddy_alloc.so
gcc -std=c11 -g -O0 buddy_alloc_test.o -o buddy_alloc_test -L. -Wl,-rpath=. -lbuddy_alloc



