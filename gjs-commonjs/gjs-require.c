/*
 * gjs-require.c
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

#include <gio/gio.h>
#include <gjs/gjs.h>

#include "gjs-require.h"

G_DEFINE_TYPE (GjsRequire, gjs_require, G_TYPE_OBJECT)

#define GJS_REQUIRE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
                                      GJS_TYPE_REQUIRE, \
                                      GjsRequirePrivate))

#define SELF_DATA_KEY "gjs-commonjs/require-obj"

#define CX (self->priv->context)
#define CX_GET_SELF(cx) (g_object_get_data (G_OBJECT (JS_GetContextPrivate (cx)), SELF_DATA_KEY))

#define GI_MODULE_PREFIX "/gi/"

/* private data */
struct _GjsRequirePrivate
{
  JSContext *context;

  JSObject *exported_paths;
  JSObject *private_paths;

  GQueue *dir_stack;

  GHashTable *module_cache;
};

/* properties */
enum
{
  PROP_0,
  PROP_CONTEXT
};

static void     gjs_require_class_init         (GjsRequireClass *class);
static void     gjs_require_init               (GjsRequire *self);

static void     gjs_require_finalize           (GObject *obj);

static void     gjs_require_set_property       (GObject      *obj,
                                                guint         prop_id,
                                                const GValue *value,
                                                GParamSpec   *pspec);
static void     gjs_require_get_property       (GObject    *obj,
                                                guint       prop_id,
                                                GValue     *value,
                                                GParamSpec *pspec);

static void     free_dir_stack_entry           (gpointer data,
                                                gpointer user_data);

static void     define_require_function        (GjsRequire *self);
static void     create_search_paths            (GjsRequire *self);

static void
gjs_require_class_init (GjsRequireClass *class)
{
  GObjectClass *obj_class = G_OBJECT_CLASS (class);

  obj_class->finalize = gjs_require_finalize;
  obj_class->get_property = gjs_require_get_property;
  obj_class->set_property = gjs_require_set_property;

  g_object_class_install_property (obj_class, PROP_CONTEXT,
                                   g_param_spec_pointer ("context",
                                                         "Gjs context",
                                                         "The Gjs context object to define 'require' API into",
                                                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                                                         G_PARAM_STATIC_STRINGS));


  /* add private structure */
  g_type_class_add_private (obj_class, sizeof (GjsRequirePrivate));
}

static void
gjs_require_init (GjsRequire *self)
{
  GjsRequirePrivate *priv;

  priv = GJS_REQUIRE_GET_PRIVATE (self);
  self->priv = priv;

  priv->dir_stack = g_queue_new ();


}

static void
gjs_require_finalize (GObject *obj)
{
  GjsRequire *self = GJS_REQUIRE (obj);

  g_queue_foreach (self->priv->dir_stack, free_dir_stack_entry, NULL);
  g_queue_free (self->priv->dir_stack);

  G_OBJECT_CLASS (gjs_require_parent_class)->finalize (obj);
}

static void
gjs_require_set_property (GObject      *obj,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  GjsRequire *self;

  self = GJS_REQUIRE (obj);

  switch (prop_id)
    {

    case PROP_CONTEXT:
      {
        GjsContext *gjs_cx;

        CX = g_value_get_pointer (value);

        gjs_cx = JS_GetContextPrivate (CX);
        g_assert (GJS_IS_CONTEXT (gjs_cx));
        g_object_set_data (G_OBJECT (gjs_cx),
                           SELF_DATA_KEY,
                           self);

        create_search_paths (self);
        define_require_function (self);

        gjs_require_push_to_private_paths (self, ".");
        gjs_require_push_to_private_paths (self, JS_LIB_DIR);

        break;
      }

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, pspec);
      break;
    }
}

static void
gjs_require_get_property (GObject    *obj,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  GjsRequire *self;

  self = GJS_REQUIRE (obj);

  switch (prop_id)
    {
    case PROP_CONTEXT:
      g_value_set_pointer (value, self->priv->context);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, pspec);
      break;
    }
}

static void
free_dir_stack_entry (gpointer data,
                      gpointer user_data)
{
  g_free (data);
}

static JSBool
seal_object_property (JSContext *context, JSObject *obj, const char *name)
{
  JSBool found;
  uintN attrs;

  if (! JS_GetPropertyAttributes (context,
                                  obj,
                                  name,
                                  &attrs,
                                  &found) || ! found)
    {
      return JS_FALSE;
    }

  attrs |= JSPROP_PERMANENT | JSPROP_READONLY;

  if (! JS_SetPropertyAttributes (context,
                                  obj,
                                  name,
                                  attrs, &found) || ! found)
    {
      return JS_FALSE;
    }

  return JS_TRUE;
}

static gboolean
module_id_is_relative (const gchar *module_name)
{
  return (g_strstr_len (module_name, 2, "./") == module_name ||
          g_strstr_len (module_name, 3, "../") == module_name);
}

static gchar *
normalize_file_name (const gchar *file_name, const gchar *parent_file_name)
{
  GFile *parent_file = NULL;
  GFile *file;
  gchar *result;

  file = g_file_new_for_path (file_name);

  if (parent_file_name != NULL)
    {
      parent_file = g_file_new_for_path (parent_file_name);
      result = g_file_get_relative_path (parent_file, file);
    }
  else
    {
      result = g_file_get_path (file);
    }

  if (parent_file != NULL)
    g_object_unref (parent_file);

  g_object_unref (file);

  return result;
}

static gboolean
search_module_in_paths (JSContext    *cx,
                        JSObject     *paths,
                        const gchar  *module_name,
                        gchar       **script,
                        gsize        *script_len,
                        gchar       **file_name,
                        gchar       **search_path,
                        GError      **error)
{
  guint len;
  gint i;
  const gchar *path;

  JS_GetArrayLength (cx, paths, &len);

  for (i = 0; i < len; i++)
    {
      JSString *st;
      jsval val;

      JS_GetElement (cx, paths, i, &val);
      st = JS_ValueToString (cx, val);
      path = JS_EncodeString (cx, st);

      *file_name = g_strdup_printf ("%s/%s", path, module_name);

      if (g_file_get_contents (*file_name,
                               script,
                               script_len,
                               error))
        {
          *search_path = g_strdup (path);
          return TRUE;
        }
      else
        {
          if ((*error)->code == G_FILE_ERROR_NOENT ||
              (*error)->code == G_FILE_ERROR_ISDIR)
            {
              g_clear_error (error);
            }
          else
            return FALSE;
        }
    }

  g_set_error (error,
               G_FILE_ERROR,
               G_FILE_ERROR_NOENT,
               "Module '%s' was not found in search paths",
               module_name);

  return FALSE;
}

static gboolean
load_module_from_file (GjsRequire   *self,
                       const gchar  *module_id,
                       gchar       **script,
                       gsize        *script_len,
                       gchar       **file_name,
                       gchar       **search_path,
                       GError      **error)
{
  gchar *module_id_dot_js;
  gboolean result = FALSE;

  /* add .js to module id if not present */
  if (g_strcmp0 (module_id + strlen (module_id) - 3, ".js") == 0)
    module_id_dot_js = g_strdup (module_id);
  else
    module_id_dot_js = g_strdup_printf ("%s.js", module_id);

  if (module_id_is_relative (module_id))
    {
      const gchar *current_module_dir;
      gchar *tmp_file_name;

      current_module_dir = g_queue_peek_head (self->priv->dir_stack);
      if (current_module_dir == NULL)
        current_module_dir = ".";

      tmp_file_name = g_strdup_printf ("%s/%s",
                                       current_module_dir,
                                       module_id_dot_js);

      *file_name = normalize_file_name (tmp_file_name, ".");
      g_free (tmp_file_name);

      if (! g_file_get_contents (*file_name,
                                 script,
                                 script_len,
                                 error))
        {
          goto out;
        }

      *search_path = g_strdup (".");

      result = TRUE;
    }
  else
    {
      if (! search_module_in_paths (CX,
                                    self->priv->exported_paths,
                                    module_id_dot_js,
                                    script,
                                    script_len,
                                    file_name,
                                    search_path,
                                    error))
        {
          if ((*error)->code == G_FILE_ERROR_NOENT)
            {
              g_clear_error (error);
              if (! search_module_in_paths (CX,
                                            self->priv->private_paths,
                                            module_id_dot_js,
                                            script,
                                            script_len,
                                            file_name,
                                            search_path,
                                            error))
                {
                  goto out;
                }
            }
          else
            {
              goto out;
            }
        }

      result = TRUE;
    }

 out:
  g_free (module_id_dot_js);

  return result;
}

static JSObject *
add_module_property_to_module_scope (JSContext   *cx,
                                     JSObject    *scope,
                                     const gchar *module_id)
{
  JSObject *module;
  jsval value;
  JSString *st;

  /* create the 'module' object */
  module = JS_NewObject (cx, NULL, NULL, NULL);
  g_assert (module != NULL);

  /* add 'module' object as property of scope */
  value = OBJECT_TO_JSVAL (module);
  g_assert (JS_SetProperty (cx, scope, "module", &value));

  /* make 'exports' a permanent and read-only property of scope */
  seal_object_property (cx, scope, "module");

  /* add 'id' property to module object */
  st = JS_NewStringCopyN (cx, module_id, strlen (module_id));
  value = STRING_TO_JSVAL (st);
  g_assert (JS_SetProperty (cx, module, "id", &value));

  /* freeze module object */
  JS_FreezeObject (cx, module);

  return module;
}

static JSObject *
add_exports_property_to_module_scope (JSContext   *cx,
                                      JSObject    *scope)
{
  JSObject *exports;
  jsval value;

  /* create the 'exports' object, into which API will be added */
  exports = JS_NewObject (cx, NULL, NULL, NULL);
  g_assert (exports != NULL);

  /* add 'exports' object as property of scope */
  value = OBJECT_TO_JSVAL (exports);
  g_assert (JS_SetProperty (cx, scope, "exports", &value));

  /* make 'exports' a permanent and read-only property of scope */
  seal_object_property (cx, scope, "exports");

  return exports;
}

static JSBool
require_func_callback (JSContext *cx, uintN argc, jsval *vp)
{
  GjsRequire *self;

  JSBool result = FALSE;
  JSObject *scope = NULL;
  JSObject *exports;
  JSString *st;

  GError *error = NULL;
  gboolean is_gi_module;
  const gchar *module_id = NULL;
  gchar *top_level_module_id = NULL;
  gchar *module_dir;
  gchar *file_name = NULL;
  gchar *absolute_file_name = NULL;
  gchar *script = NULL;
  gsize script_len;

  jsval retval;

  self = CX_GET_SELF (cx);

  /* obtain the module name from argument 0 */
  st = JS_ValueToString (cx, JS_ARGV (cx, vp) [0]);
  module_id = JS_EncodeString (cx, st);

  /* check whether it is a G-I or a plain JS module */
  if (g_strstr_len (module_id,
                    strlen (GI_MODULE_PREFIX),
                    GI_MODULE_PREFIX) == module_id)
    {
      is_gi_module = TRUE;
      module_id = module_id + strlen (GI_MODULE_PREFIX);

      script = g_strdup_printf ("imports.gi['%s']", module_id);
      script_len = strlen (script);

      scope = JS_GetGlobalObject (cx);
    }
  else
    {
      gchar *search_path = NULL;

      is_gi_module = FALSE;

      if (! load_module_from_file (self,
                                   module_id,
                                   &script,
                                   &script_len,
                                   &file_name,
                                   &search_path,
                                   &error))
        {
          /* module not found. Do a tast try looking for module as
             imports[module_id] */
          if (error->code == G_FILE_ERROR_NOENT ||
              error->code == G_FILE_ERROR_ISDIR)
            {
              is_gi_module = TRUE;

              script = g_strdup_printf ("imports['%s']", module_id);
              script_len = strlen (script);

              scope = JS_GetGlobalObject (cx);

              /* @TODO: Improve this code. Using a 'goto' here is ugly */
              goto eval_script;
            }
          else
            {
              JS_ReportError (cx, error->message);
              g_error_free (error);

              goto out;
            }
        }

      g_assert (file_name != NULL);
      g_assert (search_path != NULL);

      /* obtain current module's dir and push it to the stack */
      absolute_file_name = normalize_file_name (file_name, NULL);
      g_assert (absolute_file_name != NULL);

      module_dir = g_path_get_dirname (absolute_file_name);
      g_queue_push_head (self->priv->dir_stack, module_dir);

      /* create scope object */
      scope = JS_NewObject (cx, NULL, NULL, NULL);
      g_assert (scope != NULL);

      /* add 'module' object to scope */
      top_level_module_id = normalize_file_name (absolute_file_name, search_path);
      g_assert (top_level_module_id != NULL);

      add_module_property_to_module_scope (cx, scope, top_level_module_id);

      g_free (search_path);

      /* add 'exports' object to scope */
      exports = add_exports_property_to_module_scope (cx, scope);
    }

 eval_script:

  /* evaluate script */
  if (! JS_EvaluateScript(cx,
                          scope,
                          script,
                          script_len,
                          absolute_file_name,
                          1, /* line number */
                          &retval))
    {
      goto out;
    }

  if (is_gi_module)
    JS_RVAL (cx, vp) = retval;
  else
    JS_RVAL (cx, vp) = OBJECT_TO_JSVAL (exports);

  result = JS_TRUE;

 out:

  if (! is_gi_module)
    {
      /* pop current module's dir from the stack */
      module_dir = g_queue_pop_head (self->priv->dir_stack);
      g_free (module_dir);

      g_free (top_level_module_id);
      g_free (file_name);
      g_free (absolute_file_name);
    }

  g_free (script);

  return result;
}

static void
define_require_function (GjsRequire *self)
{
  JSFunction *func;
  JSObject *func_obj;
  jsval value;

  /* define the native 'require' function */
  func = JS_DefineFunction (CX,
                            JS_GetGlobalObject (CX),
                            "require",
                            require_func_callback,
                            1,
                            JSPROP_READONLY | JSPROP_PERMANENT);
  g_assert (func != NULL);

  /* get the object of 'require' function */
  func_obj = JS_GetFunctionObject (func);
  g_assert (func_obj != NULL);

  /* add 'paths' array as a property of 'require' function */
  value = OBJECT_TO_JSVAL (self->priv->exported_paths);
  JS_SetProperty (CX, func_obj, "paths", &value);

  /* prevent 'paths' property from being modified or removed */
  seal_object_property (CX, func_obj, "paths");

  /* seal object of 'require' function */
  JS_FreezeObject (CX, func_obj);
}

static void
create_search_paths (GjsRequire *self)
{
  /* create exported (accessible from JS) 'paths' array */
  self->priv->exported_paths = JS_NewArrayObject (CX, 0, NULL);
  g_assert (self->priv->exported_paths != NULL);

  /* create private paths array */
  self->priv->private_paths = JS_NewArrayObject (CX, 0, NULL);
  g_assert (self->priv->private_paths != NULL);
}

static void
push_path_to_array (JSContext *cx, JSObject *paths, const gchar *path)
{
  jsval val;
  JSString *st;
  guint len;

  st = JS_NewStringCopyN (cx, path, strlen (path));
  val = STRING_TO_JSVAL (st);

  JS_GetArrayLength(cx, paths, &len);
  JS_SetElement (cx, paths, len, &val);
}

/* public methods */

GjsRequire *
gjs_require_new (JSContext *context)
{
  g_return_val_if_fail (context != NULL, NULL);

  return g_object_new (GJS_TYPE_REQUIRE,
                       "context", context,
                       NULL);
}

void
gjs_require_push_to_exported_paths (GjsRequire *self, const gchar *path)
{
  g_return_if_fail (GJS_IS_REQUIRE (self));

  push_path_to_array (CX, self->priv->exported_paths, path);
}

void
gjs_require_push_to_private_paths (GjsRequire *self, const gchar *path)
{
  g_return_if_fail (GJS_IS_REQUIRE (self));

  push_path_to_array (CX, self->priv->private_paths, path);
}
