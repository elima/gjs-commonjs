# configure.ac
#
# gjs-commonjs, CommonJS support for the GNOME Javascript engine
#
# Copyright (C) 2011, Igalia S.L.
#
# Authors:
#   Eduardo Lima Mitev <elima@igalia.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Affero General Public License
# version 3, or (at your option) any later version as published by
# the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Affero General Public License at http://www.gnu.org/licenses/agpl.html
# for more details.

AC_PREREQ(2.59)

m4_define([prj_name],       [gjs-commonjs])
m4_define([prj_short_name], [gjs-commonjs])
m4_define([prj_home],       [http://people.igalia.com/elima/gjs-commonjs])

m4_define([prj_version_major], [0])
m4_define([prj_version_minor], [1])
m4_define([prj_version_micro], [1])

m4_define([prj_version],
          [prj_version_major.prj_version_minor.prj_version_micro])
m4_define([prj_api_version],
          [prj_version_major.prj_version_minor])

AC_INIT([prj_name],
	[prj_version],
	[prj_home])

AC_CONFIG_HEADERS(config.h)
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE

PRJ_VERSION="prj_version"
AC_SUBST(PRJ_VERSION)

PRJ_API_VERSION="prj_api_version"
AC_SUBST(PRJ_API_VERSION)

PRJ_API_NAME="prj_short_name-prj_api_version"
AC_SUBST(PRJ_API_NAME)

m4_define([prj_gir_namespace], [GjsCommonjs])

# Check for programs
AC_PROG_LIBTOOL
AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_CC_C_O

# Enable pkg-config
PKG_PROG_PKG_CONFIG

# Required libraries
GLIB_REQUIRED=2.23.0
PKG_CHECK_MODULES(GLIB, gio-2.0 >= $GLIB_REQUIRED
		        glib-2.0 >= $GLIB_REQUIRED
		        gthread-2.0 >= $GLIB_REQUIRED
		        gobject-2.0 >= $GLIB_REQUIRED)
PKG_CHECK_MODULES(MOZJS, mozilla-js >= 4.0)
PKG_CHECK_MODULES(GJS, gjs-1.0 >= 0.4)

# GObject-Introspection check
GOBJECT_INTROSPECTION_CHECK([0.6.7])
if test "x$found_introspection" = "xyes"; then
   GIR_NAMESPACE="prj_gir_namespace"
   AC_SUBST(GIR_NAMESPACE)

   GIR_API_NAME="prj_gir_namespace"-"prj_api_version"
   AC_SUBST(GIR_API_NAME)

   GIR_TARGET_NAME="prj_gir_namespace"_"prj_version_major"_"prj_version_minor"_gir
   AC_SUBST(GIR_TARGET_NAME)
fi

# Silent build
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

# Tests
AC_ARG_ENABLE(tests,
        AS_HELP_STRING([--enable-tests[=@<:@no/yes@:>@]],
                [Enable automated unit and functional tests [default=yes]]),,
                [enable_tests=yes])

AM_CONDITIONAL(ENABLE_TESTS, test x"${enable_tests}" = x"yes")

# Output files
AC_OUTPUT([
	Makefile
        gjs-commonjs/Makefile
        gjs-commonjs/gjs-commonjs-0.1.pc
        js/Makefile
])

echo ""
echo "   prj_name $VERSION"
echo "   ========================="
echo ""
echo "              Install prefix:   ${prefix}"
echo "      Enable automated tests:   ${enable_tests}"
echo ""
