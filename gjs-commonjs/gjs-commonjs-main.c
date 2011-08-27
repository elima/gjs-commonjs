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
      //      g_debug ("%s", error->message);
      g_error_free (error);
    }

  g_object_unref (context);

  g_debug ("clean exit!");

  return exit_status;
}
