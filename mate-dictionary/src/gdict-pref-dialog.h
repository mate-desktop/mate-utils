/* gdict-pref-dialog.h - preferences dialog
 *
 * This file is part of MATE Dictionary
 *
 * Copyright (C) 2005 Emmanuele Bassi
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef __GDICT_PREF_DIALOG_H__
#define __GDICT_PREF_DIALOG_H__

#include <gtk/gtk.h>
#include <libgdict/gdict.h>

G_BEGIN_DECLS

#define GDICT_TYPE_PREF_DIALOG		(gdict_pref_dialog_get_type ())
#define GDICT_PREF_DIALOG(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GDICT_TYPE_PREF_DIALOG, GdictPrefDialog))
#define GDICT_IS_PREF_DIALOG(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDICT_TYPE_PREF_DIALOG))

#define GDICT_DEFAULT_DEFBOX_FONT 	"Sans 10"
#define GDICT_DEFAULT_PRINT_FONT 	"Serif 10"
#define GDICT_DEFAULT_SOURCE_NAME 	"Default"

#define GDICT_SETTINGS_SCHEMA           "org.mate.dictionary"
#define GDICT_SETTINGS_DATABASE_KEY 	"database"
#define GDICT_SETTINGS_STRATEGY_KEY 	"strategy"
#define GDICT_SETTINGS_PRINT_FONT_KEY 	"print-font"
#define GDICT_SETTINGS_SOURCE_KEY 	"source-name"

#define DESKTOP_SETTINGS_SCHEMA         "org.mate.interface"
#define DOCUMENT_FONT_KEY               "document-font-name"

typedef struct _GdictPrefDialog        GdictPrefDialog;
typedef struct _GdictPrefDialogClass   GdictPrefDialogClass;

GType gdict_pref_dialog_get_type (void) G_GNUC_CONST;

void gdict_show_pref_dialog (GtkWidget         *parent,
			     const gchar       *title,
			     GdictSourceLoader *loader);

G_END_DECLS

#endif /* __GDICT_PREF_DIALOG_H__ */
