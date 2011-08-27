/*
 * gjs-commonjs-context.c
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

#include <gjs/gjs.h>

#include "gjs-commonjs-context.h"

G_DEFINE_TYPE (GjsCommonjsContext, gjs_commonjs_context, G_TYPE_OBJECT)

#define GJS_COMMONJS_CONTEXT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
                                               GJS_TYPE_COMMONJS_CONTEXT, \
                                               GjsCommonjsContextPrivate))

/* private data */
struct _GjsCommonjsContextPrivate
{
  GjsContext *context;
  GjsRequire *require;
};

/* properties */
enum
{
  PROP_0,
  PROP_CONTEXT,
  PROP_REQUIRE
};

static void     gjs_commonjs_context_class_init         (GjsCommonjsContextClass *class);
static void     gjs_commonjs_context_init               (GjsCommonjsContext *self);

static void     gjs_commonjs_context_finalize           (GObject *obj);

static void     gjs_commonjs_context_set_property       (GObject      *obj,
                                                         guint         prop_id,
                                                         const GValue *value,
                                                         GParamSpec   *pspec);
static void     gjs_commonjs_context_get_property       (GObject    *obj,
                                                         guint       prop_id,
                                                         GValue     *value,
                                                         GParamSpec *pspec);

static void
gjs_commonjs_context_class_init (GjsCommonjsContextClass *class)
{
  GObjectClass *obj_class = G_OBJECT_CLASS (class);

  obj_class->finalize = gjs_commonjs_context_finalize;
  obj_class->get_property = gjs_commonjs_context_get_property;
  obj_class->set_property = gjs_commonjs_context_set_property;

  g_object_class_install_property (obj_class, PROP_CONTEXT,
                                   g_param_spec_object ("context",
                                                        "Gjs Context",
                                                        "The #GjsContext object wrapped by this context",
                                                        GJS_TYPE_CONTEXT,
                                                        G_PARAM_READABLE |
                                                        G_PARAM_STATIC_STRINGS));

  g_object_class_install_property (obj_class, PROP_CONTEXT,
                                   g_param_spec_object ("require",
                                                        "Gjs Require",
                                                        "The #GjsRequire object wrapped by this context",
                                                        GJS_TYPE_REQUIRE,
                                                        G_PARAM_READABLE |
                                                        G_PARAM_STATIC_STRINGS));


  /* add private structure */
  g_type_class_add_private (obj_class, sizeof (GjsCommonjsContextPrivate));
}

static void
gjs_commonjs_context_init (GjsCommonjsContext *self)
{
  GjsCommonjsContextPrivate *priv;
  JSContext *cx;

  priv = GJS_COMMONJS_CONTEXT_GET_PRIVATE (self);
  self->priv = priv;

  priv->context = gjs_context_new ();

  cx = gjs_context_get_native_context (priv->context);

  priv->require = gjs_require_new (cx);
}

static void
gjs_commonjs_context_finalize (GObject *obj)
{
  GjsCommonjsContext *self = GJS_COMMONJS_CONTEXT (obj);

  g_object_unref (self->priv->require);
  g_object_unref (self->priv->context);

  G_OBJECT_CLASS (gjs_commonjs_context_parent_class)->finalize (obj);
}

static void
gjs_commonjs_context_set_property (GObject      *obj,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  //  GjsCommonjsContext *self;

  //  self = GJS_COMMONJS_CONTEXT (obj);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, pspec);
      break;
    }
}

static void
gjs_commonjs_context_get_property (GObject    *obj,
                                   guint       prop_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
  GjsCommonjsContext *self;

  self = GJS_COMMONJS_CONTEXT (obj);

  switch (prop_id)
    {
    case PROP_CONTEXT:
      g_value_take_object (value, self->priv->context);
      break;

    case PROP_REQUIRE:
      g_value_take_object (value, self->priv->require);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, pspec);
      break;
    }
}

/* public methods */

GjsCommonjsContext *
gjs_commonjs_context_new ()
{
  return g_object_new (GJS_TYPE_COMMONJS_CONTEXT,
                       NULL);
}

gboolean
gjs_commonjs_context_eval_file (GjsCommonjsContext  *self,
                                const gchar         *filename,
                                gint                *exit_status,
                                GError             **error)
{
  g_return_val_if_fail (GJS_IS_COMMONJS_CONTEXT (self), FALSE);

  return gjs_context_eval_file (self->priv->context,
                                filename,
                                exit_status,
                                error);
}

gboolean
gjs_commonjs_context_eval (GjsCommonjsContext  *self,
                           const char          *script,
                           gssize               script_len,
                           const char          *filename,
                           gint                *exit_status,
                           GError             **error)
{
  g_return_val_if_fail (GJS_IS_COMMONJS_CONTEXT (self), FALSE);

  return gjs_context_eval (self->priv->context,
                           script,
                           script_len,
                           filename,
                           exit_status,
                           error);
}

GjsContext *
gjs_commonjs_context_get_context (GjsCommonjsContext *self)
{
  g_return_val_if_fail (GJS_IS_COMMONJS_CONTEXT (self), NULL);

  return self->priv->context;
}

GjsRequire *
gjs_commonjs_context_get_require (GjsCommonjsContext *self)
{
  g_return_val_if_fail (GJS_IS_COMMONJS_CONTEXT (self), NULL);

  return self->priv->require;
}
