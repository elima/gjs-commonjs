MAINTAINERCLEANFILES = \
	Makefile.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = @PRJ_API_NAME@.pc

library_includedir=$(includedir)/@PRJ_API_NAME@
library_include_HEADERS = gjs-commonjs.h

jslibdir = $(libdir)/@PRJ_API_NAME@/js

CLEANFILES = *~ @PRJ_API_NAME@.pc

AM_CFLAGS = -Wall -Werror -g3 -O0 -ggdb \
	-DPKGDATADIR="\"$(pkgdatadir)\"" \
	-DJS_LIB_DIR="\"$(jslibdir)\"" \
	-DENABLE_TESTS="\"$(enable_tests)\""

# libraries
lib_LTLIBRARIES = lib@PRJ_API_NAME@.la

# libgjs-commonjs
source_c = \
	gjs-require.c \
	gjs-commonjs-context.c

source_h = \
	gjs-require.h \
	gjs-commonjs-context.h

source_h_priv =

lib@PRJ_API_NAME@_la_LIBADD = \
	$(GLIB_LIBS) \
	$(MOZJS_LIBS) \
	$(GJS_LIBS)

lib@PRJ_API_NAME@_la_CFLAGS  = \
	$(AM_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(MOZJS_CFLAGS) \
	$(GJS_CFLAGS)

lib@PRJ_API_NAME@_la_LDFLAGS = \
	-version-info 0:1:0 \
	-no-undefined

lib@PRJ_API_NAME@_la_SOURCES = \
	$(source_c) \
	$(source_h) \
	$(source_h_priv)

prjdir = $(includedir)/@PRJ_API_NAME@
prj_HEADERS = \
	$(source_h) \
	$(source_h_priv)

# introspection support
if HAVE_INTROSPECTION

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir) --warn-all
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

introspection_sources = $(prj_HEADERS) $(source_c)

@GIR_API_NAME@.gir: lib@PRJ_API_NAME@.la Makefile
@GIR_TARGET_NAME@_INCLUDES = GLib-2.0 GObject-2.0 Gio-2.0
@GIR_TARGET_NAME@_CFLAGS = $(INCLUDES) $(MOZJS_CFLAGS) $(GJS_CFLAGS)
@GIR_TARGET_NAME@_LIBS = @PRJ_API_NAME@
@GIR_TARGET_NAME@_FILES = $(addprefix $(srcdir)/,$(introspection_sources))

INTROSPECTION_GIRS += @GIR_API_NAME@.gir

girdir = $(datadir)/gir-1.0
dist_gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

# gjs-commonjs
bin_PROGRAMS = gjs-commonjs

gjs_commonjs_CFLAGS = \
	$(AM_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(MOZJS_CFLAGS) \
	$(GJS_CFLAGS) \
	-L./

gjs_commonjs_LDADD = \
	$(GLIB_LIBS) \
	$(MOZJS_LIBS) \
	$(GJS_LIBS) \
	-l@PRJ_API_NAME@

gjs_commonjs_SOURCES = \
	gjs-commonjs-main.c

CLEANFILES += $(dist_gir_DATA) $(typelib_DATA)
endif

maintainer-clean-local:
	rm -rf tmp-introspect*
