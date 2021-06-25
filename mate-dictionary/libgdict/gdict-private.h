/* Copyright (C) 2006 Emmanuele Bassi <ebassi@gmail.com>
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

#include <gtk/gtk.h>
#include "gdict-debug.h"

G_BEGIN_DECLS

gboolean _gdict_has_ipv6 (void);

void _gdict_show_error_dialog  (GtkWidget   *widget,
			        const gchar *title,
			        const gchar *detail);
void _gdict_show_gerror_dialog (GtkWidget   *widget,
			        const gchar *title,
			        GError      *error);

G_END_DECLS
