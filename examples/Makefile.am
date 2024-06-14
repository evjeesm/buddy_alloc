

bin_PROGRAMS = adapter_demo heap_example mmap_example

adapter_demo_SOURCES = adapter_demo.c vector.h
adapter_demo_CFLAGS = -g -I../vector/src
adapter_demo_LDFLAGS = 
adapter_demo_LDADD = adapted_vector_r.o ../libbuddy_alloc.la
EXTRA_adapter_demo_DEPENDENCIES = ../vector/src/libvector_static.la adapted_vector_r.o

# making partialy linked object in order to convince vector to interface with adapter.
adapted_vector_r.o: ../libbuddy_alloc_vec_adapt.la
	ld -L.. -L../.libs -L../vector/src/.libs -r --whole-archive -lvector_static -lbuddy_alloc_vec_adapt -o $@


heap_example_SOURCES = heap_example.c
heap_example_CFLAGS = -I..
heap_example_LDADD = ../libbuddy_alloc.la

mmap_example_SOURCES = mmap_example.c
mmap_example_CFLAGS = -I..
mmap_example_LDADD = ../libbuddy_alloc.la
