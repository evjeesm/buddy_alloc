SUBDIRS = vector . examples

lib_LTLIBRARIES = libbuddy_alloc.la libbuddy_alloc_vec_adapt.la
libbuddy_alloc_la_SOURCES = buddy_alloc.c buddy_alloc.h
libbuddy_alloc_la_LDFLAGS = -static
libbuddy_alloc_vec_adapt_la_SOURCES = buddy_alloc_vec_adapt.c buddy_alloc_vec_adapt.h

VALGRIND_memcheck_FLAGS = --leak-check=full --track-origins=yes
@VALGRIND_CHECK_RULES@

TESTS = buddy_alloc_test
check_PROGRAMS = buddy_alloc_test

buddy_alloc_test_SOURCES = buddy_alloc_test.c $(top_builddir)/buddy_alloc.h
buddy_alloc_test_CFLAGS = @CHECK_CFLAGS@
buddy_alloc_test_LDADD = $(top_builddir)/libbuddy_alloc.la @CHECK_LIBS@

include_HEADERS = buddy_alloc.h bd_region.h bd_static_arena.h

debug-test: libbuddy_alloc.la buddy_alloc_test
	LD_LIBRARY_PATH=./.libs:/usr/local/lib CK_FORK=no gdb -tui buddy_alloc_test

