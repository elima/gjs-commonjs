/*
 * test-js-all.c
 *
 * gjs-commonjs, CommonJS support for the GNOME Javascript engine
 *
 * Copyright (C) 2011, Igalia S.L.
 *
 * Authors:
 *   Eduardo Lima Mitev <elima@igalia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 3, or (at your option) any later version as published by
 * the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License at http://www.gnu.org/licenses/lgpl-3.0.txt
 * for more details.
 */

#include <gjs-commonjs/gjs-commonjs.h>

typedef struct
{
  GjsCommonjsContext *context;
} GjsTestJSFixture;

gchar *js_test_dir;

static void
setup (GjsTestJSFixture *fix,
       gconstpointer     test_data)
{
  GjsRequire *require;

  fix->context = gjs_commonjs_context_new ();
  require = gjs_commonjs_context_get_require (fix->context);

  gjs_require_push_to_private_paths (require, TESTS_DIR "/js");
}

static void
teardown (GjsTestJSFixture *fix,
          gconstpointer     test_data)
{
  g_object_unref (fix->context);
}

static void
test (GjsTestJSFixture *fix,
      gconstpointer     test_data)
{
  GError *error = NULL;
  gint code;

  gjs_commonjs_context_eval_file (fix->context,
                                  test_data,
                                  &code,
                                  &error);
  g_free ((gchar *) test_data);

  if (error != NULL)
    {
      /* @TODO: here we should decide if a failing test aborts
         the process. By now, just log and continue. */
      g_error ("%s", error->message);
      g_error_free (error);
    }
}

gint
main (gint argc, gchar *argv[])
{
  gchar *test_dir;
  const gchar *name;
  GDir *dir;

  g_test_init (&argc, &argv, NULL);
  g_type_init ();

  test_dir = g_path_get_dirname (argv[0]);
  js_test_dir = g_build_filename (TESTS_DIR, "js", NULL);
  g_free (test_dir);

  /* iterate through all 'test*.js' files in 'js_test_dir' */
  dir = g_dir_open (js_test_dir, 0, NULL);
  g_assert (dir != NULL);

  while ((name = g_dir_read_name (dir)) != NULL) {
    gchar *test_name;
    gchar *file_name;

    if (! (g_str_has_prefix (name, "test") &&
           g_str_has_suffix (name, ".js")))
      continue;

    /* pretty print, drop 'test' prefix and '.js' suffix from test name */
    test_name = g_strconcat ("/js/", name + 4, NULL);
    test_name[strlen (test_name)-3] = '\0';

    file_name = g_build_filename (js_test_dir, name, NULL);
    g_test_add (test_name, GjsTestJSFixture, file_name, setup, test, teardown);

    g_free (test_name);
    /* not freeing file_name as it's needed while running the test */
  }
  g_dir_close (dir);

  return g_test_run ();
}
