/* Copyright (C) 2005 Emmanuele Bassi
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

#ifndef __GDICT_COMMON_H__
#define __GDICT_COMMON_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

gboolean gdict_create_data_dir (void);
gchar *  gdict_get_data_dir    (void) G_GNUC_MALLOC;

void     gdict_show_error_dialog  (GtkWindow   *parent,
				   const gchar *message,
				   const gchar *detail);
void     gdict_show_gerror_dialog (GtkWindow   *parent,
				   const gchar *message,
				   GError      *error);

G_END_DECLS

#endif /* __GDICT_COMMON_H__ */
