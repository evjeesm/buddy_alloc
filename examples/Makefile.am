

bin_PROGRAMS = adapter_demo heap_example mmap_example

adapter_demo_SOURCES = adapter_demo.c $(top_srcdir)/buddy_alloc_vec_adapt.c $(top_srcdir)/bd_static_arena.h 
adapter_demo_CFLAGS = -g -I$(top_srcdir) -I$(top_srcdir)/vector/src
adapter_demo_LDADD = $(top_builddir)/libbuddy_alloc.la $(top_builddir)/vector/src/libvector_static.la
# EXTRA_adapter_demo_DEPENDENCIES = $(top_builddir)/vector/src/libvector_static.la adapted_vector_r.o
#
# # making partialy linked object in order to convince vector to interface with adapter.
# adapted_vector_r.o: $(top_builddir)/libbuddy_alloc_vec_adapt.la
# 	/bin/bash ../libtool --tag=CC --mode=compile gcc ../libbuddy_alloc_vec_adapt.la ../vector/src/libvector_static.la -o $@


heap_example_SOURCES = heap_example.c
heap_example_CFLAGS = -I$(top_srcdir)
heap_example_LDADD = $(top_builddir)/libbuddy_alloc.la

mmap_example_SOURCES = mmap_example.c
mmap_example_CFLAGS = -I$(top_srcdir)
mmap_example_LDADD = $(top_builddir)/libbuddy_alloc.la
