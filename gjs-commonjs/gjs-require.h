/*
 * gjs-require.h
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

#ifndef __GJS_REQUIRE_H__
#define __GJS_REQUIRE_H__

#include <glib-object.h>
#include <jsapi.h>

G_BEGIN_DECLS

typedef struct _GjsRequire GjsRequire;
typedef struct _GjsRequireClass GjsRequireClass;
typedef struct _GjsRequirePrivate GjsRequirePrivate;

struct _GjsRequire
{
  GObject parent;

  GjsRequirePrivate *priv;
};

struct _GjsRequireClass
{
  GObjectClass parent_class;
};

#define GJS_TYPE_REQUIRE           (gjs_require_get_type ())
#define GJS_REQUIRE(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GJS_TYPE_REQUIRE, GjsRequire))
#define GJS_REQUIRE_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), GJS_TYPE_REQUIRE, GjsRequireClass))
#define GJS_IS_REQUIRE(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GJS_TYPE_REQUIRE))
#define GJS_IS_REQUIRE_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), GJS_TYPE_REQUIRE))
#define GJS_REQUIRE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GJS_TYPE_REQUIRE, GjsRequireClass))


GType        gjs_require_get_type                (void) G_GNUC_CONST;

GjsRequire * gjs_require_new                     (JSContext *context);

void         gjs_require_push_to_exported_paths  (GjsRequire  *self,
                                                  const gchar *path);
void         gjs_require_push_to_private_paths   (GjsRequire  *self,
                                                  const gchar *path);

G_END_DECLS

#endif /* __GJS_REQUIRE_H__ */
