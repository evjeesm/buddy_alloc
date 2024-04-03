#!/bin/sh

# gcc -std=c2x -g -fpic -c buddy_alloc.c
# gcc -std=c2x -g -fpic -c buddy_alloc_front.c
# gcc -std=c2x -g -fpic -c byddy_alloc_test.c
# gcc -std=c2x -g -fpic -shared buddy_alloc.o buddy_alloc_front.o -o libbuddy_alloc.so
# 
# gcc -std=c2x -g buddy_alloc_test.o -o buddy_alloc_test -L. -Wl,-rpath=. -lbuddy_alloc
#
#

gcc -std=c11 -g -O0 buddy_alloc.c -o main
