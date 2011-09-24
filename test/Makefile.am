MAINTAINERCLEANFILES = \
	Makefile.in

tests_dir = `pwd`/

AM_CFLAGS = \
	-Wall -g3 -O0 -ggdb \
	-DTESTS_DIR="\"$(tests_dir)\"" \
	$(GLIB_CFLAGS) \
	$(MOZJS_CFLAGS) \
	$(GJS_CFLAGS) \
	-I$(top_srcdir)/gjs-commonjs

AM_LIBS = \
	$(GLIB_LIBS) \
	$(MOZJS_LIBS) \
	$(GJS_LIBS) \
	$(top_srcdir)/gjs-commonjs/lib@PRJ_API_NAME@.la

if ENABLE_TESTS

noinst_PROGRAMS = \
	test-all-js

TESTS = \
	test-all-js

# test-all-js
test_all_js_CFLAGS = $(AM_CFLAGS) $(GJS_CFLAGS)
test_all_js_LDADD = $(AM_LIBS) $(GJS_LIBS)
test_all_js_SOURCES = test-all-js.c

endif # ENABLE_TESTS
