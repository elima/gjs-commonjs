/*
 * gjs-commonjs-main.c
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

#include <gjs-commonjs-context.h>

gint
main (gint argc, gchar *argv[])
{
  GError *error = NULL;
  GjsCommonjsContext *context;
  gint exit_status;

  if (argc < 2)
    {
      g_print ("Usage: %s <script-file-name>\n", argv[0]);
      return -1;
    }

  g_type_init ();

  context = gjs_commonjs_context_new ();

  if (! gjs_commonjs_context_eval_file (context,
                                        argv[1],
                                        &exit_status,
                                        &error))
    {
      g_error ("%s", error->message);
      g_error_free (error);
    }

  g_object_unref (context);

  return exit_status;
}
