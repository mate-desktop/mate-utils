/* Copyright (C) 2006 Paolo Borelli
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

#ifndef __BAOBAB_CELL_RENDERER_PROGRESS_H__
#define __BAOBAB_CELL_RENDERER_PROGRESS_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define BAOBAB_TYPE_CELL_RENDERER_PROGRESS (baobab_cell_renderer_progress_get_type ())
#define BAOBAB_CELL_RENDERER_PROGRESS(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), BAOBAB_TYPE_CELL_RENDERER_PROGRESS, BaobabCellRendererProgress))
#define BAOBAB_CELL_RENDERER_PROGRESS_CLASS(klass)	  (G_TYPE_CHECK_CLASS_CAST ((klass), BAOBAB_TYPE_CELL_RENDERER_PROGRESS, BaobabCellRendererProgressClass))
#define BAOBAB_IS_CELL_RENDERER_PROGRESS(obj)	  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BAOBAB_TYPE_CELL_RENDERER_PROGRESS))
#define BAOBAB_IS_CELL_RENDERER_PROGRESS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), BAOBAB_TYPE_CELL_RENDERER_PROGRESS))
#define BAOBAB_CELL_RENDERER_PROGRESS_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), BAOBAB_TYPE_CELL_RENDERER_PROGRESS, BaobabCellRendererProgressClass))

typedef struct _BaobabCellRendererProgress         BaobabCellRendererProgress;
typedef struct _BaobabCellRendererProgressClass    BaobabCellRendererProgressClass;
typedef struct _BaobabCellRendererProgressPrivate  BaobabCellRendererProgressPrivate;

struct _BaobabCellRendererProgress
{
  GtkCellRenderer parent_instance;

  /*< private >*/
  BaobabCellRendererProgressPrivate *priv;
};

struct _BaobabCellRendererProgressClass
{
  GtkCellRendererClass parent_class;
};

GType		 baobab_cell_renderer_progress_get_type (void) G_GNUC_CONST;
GtkCellRenderer* baobab_cell_renderer_progress_new      (void);

G_END_DECLS

#endif  /* __BAOBAB_CELL_RENDERER_PROGRESS_H__ */
