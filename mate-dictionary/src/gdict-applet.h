/* Copyright (c) 2005  Emmanuele Bassi <ebassi@gmail.com>
 * Copyright (C) 2012-2021 MATE Developers
 *
 * This file is part of MATE Utils.
 *
 * MATE Utils is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * MATE Utils is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MATE Utils.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef __GDICT_APPLET_H__
#define __GDICT_APPLET_H__

#include <gtk/gtk.h>
#include <mate-panel-applet.h>
#include <libgdict/gdict.h>

G_BEGIN_DECLS

#define GDICT_TYPE_APPLET		(gdict_applet_get_type ())
#define GDICT_APPLET(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GDICT_TYPE_APPLET, GdictApplet))
#define GDICT_IS_APPLET(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDICT_TYPE_APPLET))

typedef struct _GdictApplet        GdictApplet;
typedef struct _GdictAppletClass   GdictAppletClass;
typedef struct _GdictAppletPrivate GdictAppletPrivate;

struct _GdictApplet
{
  MatePanelApplet parent_instance;

  GdictAppletPrivate *priv;
};

GType gdict_applet_get_type (void);

G_END_DECLS

#endif /* __GDICT_APPLET_H__ */
