/* Baobab - (C) 2005 Fabio Marzocca

	baobab-remote-connect-dialog.h

   Modified module from BaobabRemoteConnectDialog.h
   Released under same licence
 */

/*
 * Caja
 *
 * Copyright (C) 2003 Red Hat, Inc.
 *
 * Caja is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * Caja is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; see the file COPYING.  If not,
 * write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef BAOBAB_REMOTE_CONNECT_DIALOG_H
#define BAOBAB_REMOTE_CONNECT_DIALOG_H

#include <gtk/gtk.h>
#include <gio/gio.h>


#define BAOBAB_TYPE_REMOTE_CONNECT_DIALOG         (baobab_remote_connect_dialog_get_type())
#define BAOBAB_REMOTE_CONNECT_DIALOG(obj)         (G_TYPE_CHECK_INSTANCE_CAST ((obj), BAOBAB_TYPE_REMOTE_CONNECT_DIALOG, BaobabRemoteConnectDialog))
#define BAOBAB_REMOTE_CONNECT_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), BAOBAB_TYPE_REMOTE_CONNECT_DIALOG, BaobabRemoteConnectDialogClass))
#define BAOBAB_IS_REMOTE_CONNECT_DIALOG(obj)      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BAOBAB_TYPE_REMOTE_CONNECT_DIALOG)

typedef struct _BaobabRemoteConnectDialog        BaobabRemoteConnectDialog;
typedef struct _BaobabRemoteConnectDialogClass   BaobabRemoteConnectDialogClass;
typedef struct _BaobabRemoteConnectDialogDetails BaobabRemoteConnectDialogDetails;

struct _BaobabRemoteConnectDialog {
	GtkDialog parent;
	BaobabRemoteConnectDialogDetails *details;
};

struct _BaobabRemoteConnectDialogClass {
	GtkDialogClass parent_class;
};

GType      baobab_remote_connect_dialog_get_type (void);
GtkWidget* baobab_remote_connect_dialog_new      (GtkWindow *window,
						  GFile *location);

#endif /* BAOBAB_REMOTE_CONNECT_DIALOG_H */
