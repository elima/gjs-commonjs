/*
 * gjs-commonjs-context.h
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

#ifndef __GJS_COMMONJS_CONTEXT_H__
#define __GJS_COMMONJS_CONTEXT_H__

#include <glib-object.h>
#include <gjs/gjs.h>

#include <gjs-require.h>

G_BEGIN_DECLS

typedef struct _GjsCommonjsContext GjsCommonjsContext;
typedef struct _GjsCommonjsContextClass GjsCommonjsContextClass;
typedef struct _GjsCommonjsContextPrivate GjsCommonjsContextPrivate;

struct _GjsCommonjsContext
{
  GObject parent;

  GjsCommonjsContextPrivate *priv;
};

struct _GjsCommonjsContextClass
{
  GObjectClass parent_class;
};

#define GJS_TYPE_COMMONJS_CONTEXT           (gjs_commonjs_context_get_type ())
#define GJS_COMMONJS_CONTEXT(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GJS_TYPE_COMMONJS_CONTEXT, GjsCommonjsContext))
#define GJS_COMMONJS_CONTEXT_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GJS_TYPE_COMMONJS_CONTEXT, GjsCommonjsContextClass))
#define GJS_IS_COMMONJS_CONTEXT(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GJS_TYPE_COMMONJS_CONTEXT))
#define GJS_IS_COMMONJS_CONTEXT_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GJS_TYPE_COMMONJS_CONTEXT))
#define GJS_COMMONJS_CONTEXT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GJS_TYPE_COMMONJS_CONTEXT, GjsCommonjsContextClass))


GType                gjs_commonjs_context_get_type                (void) G_GNUC_CONST;

GjsCommonjsContext * gjs_commonjs_context_new                     (void);

gboolean             gjs_commonjs_context_eval_file               (GjsCommonjsContext  *self,
                                                                   const gchar         *filename,
                                                                   gint                *exit_status,
                                                                   GError             **error);
gboolean             gjs_commonjs_context_eval                    (GjsCommonjsContext  *self,
                                                                   const char          *script,
                                                                   gssize               script_len,
                                                                   const char          *filename,
                                                                   gint                *exit_status,
                                                                   GError             **error);

GjsContext *         gjs_commonjs_context_get_context             (GjsCommonjsContext *self);
GjsRequire *         gjs_commonjs_context_get_require             (GjsCommonjsContext *self);

G_END_DECLS

#endif /* __GJS_COMMONJS_CONTEXT_H__ */
