ACLOCAL_AMFLAGS = -I m4 -I/usr/share/aclocal
SUBDIRS = vector/src vector .

lib_LTLIBRARIES = libbuddy_alloc.la libbuddy_alloc_vec_adapt.la
libbuddy_alloc_la_SOURCES = buddy_alloc.c buddy_alloc.h
libbuddy_alloc_la_LDFLAGS = -static
libbuddy_alloc_vec_adapt_la_SOURCES = buddy_alloc_vec_adapt.c buddy_alloc_vec_adapt.h
libbuddy_alloc_vec_adapt_la_LDFLAGS = -static

bin_PROGRAMS = adapter_demo
adapter_demo_SOURCES = adapter_demo.c vector.h
adapter_demo_CFLAGS = -g -Ivector/src
adapter_demo_LDFLAGS = 
adapter_demo_LDADD = adapted_vector_r.o libbuddy_alloc.la
EXTRA_adapter_demo_DEPENDENCIES = vector/src/libvector_static.la adapted_vector_r.o

# making partialy linked object in order to convince vector to interface with adapter.
adapted_vector_r.o: libbuddy_alloc_vec_adapt.la
	ld -L. -L.libs -Lvector/src/.libs -r --whole-archive -lvector_static -lbuddy_alloc_vec_adapt -o $@

VALGRIND_memcheck_FLAGS = --leak-check=full --track-origins=yes
@VALGRIND_CHECK_RULES@

TESTS = buddy_alloc_test
check_PROGRAMS = buddy_alloc_test

buddy_alloc_test_SOURCES = buddy_alloc_test.c $(top_builddir)/buddy_alloc.h
buddy_alloc_test_CFLAGS = @CHECK_CFLAGS@
buddy_alloc_test_LDADD = $(top_builddir)/libbuddy_alloc.la @CHECK_LIBS@

debug-test: libbuddy_alloc.la buddy_alloc_test
	LD_LIBRARY_PATH=./.libs:/usr/local/lib CK_FORK=no gdb -tui buddy_alloc_test

