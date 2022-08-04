/* Copyright (C) 2005-2006 Fabio Marzocca <thesaltydog@gmail.com>
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

#ifndef __BAOBAB_CALLBACKS_H__
#define __BAOBAB_CALLBACKS_H__

#include <gtk/gtk.h>
#include "baobab-chart.h"

void on_quit_activate (GtkMenuItem *menuitem, gpointer user_data);
void on_about_activate (GtkMenuItem *menuitem, gpointer user_data);
void on_menuscanhome_activate (GtkMenuItem *menuitem, gpointer user_data);
void on_menuallfs_activate (GtkMenuItem *menuitem, gpointer user_data);
void on_menuscandir_activate (GtkMenuItem *menuitem, gpointer user_data);
void on_menu_collapse_activate (GtkMenuItem *menuitem, gpointer user_data);
void on_menu_expand_activate (GtkMenuItem *menuitem, gpointer user_data);
void on_menu_stop_activate (GtkMenuItem *menuitem, gpointer user_data);
void on_menu_rescan_activate (GtkMenuItem *menuitem, gpointer user_data);
void on_tbscandir_clicked (GtkToolButton *toolbutton, gpointer user_data);
void on_tbscanhome_clicked (GtkToolButton *toolbutton, gpointer user_data);
void on_tbscanall_clicked (GtkToolButton *toolbutton, gpointer user_data);
void on_tbstop_clicked (GtkToolButton *toolbutton, gpointer user_data);
void on_tbrescan_clicked (GtkToolButton *toolbutton, gpointer user_data);
void on_radio_allfs_clicked (GtkButton *button, gpointer user_data);
void on_radio_dir_clicked (GtkButton *button, gpointer user_data);
gboolean on_delete_activate (GtkWidget *widget, GdkEvent *event, gpointer user_data);
void open_file_cb (GtkMenuItem *pmenu, gpointer dummy);
void scan_folder_cb (GtkMenuItem *pmenu, gpointer dummy);
void trash_dir_cb (GtkMenuItem *pmenu, gpointer dummy);
void list_all_cb (GtkMenuItem *pmenu, gpointer dummy);
void on_pref_menu (GtkAction *a, gpointer user_data);
void on_tb_scan_remote_clicked (GtkToolButton *toolbutton, gpointer user_data);
void on_menu_scan_rem_activate (GtkMenuItem *menuitem, gpointer user_data);
void on_ck_allocated_activate (GtkToggleAction *action, gpointer user_data);
void on_helpcontents_activate (GtkAction *a, gpointer user_data);
void on_tv_selection_changed (GtkTreeSelection *selection, gpointer user_data);
void on_move_upwards_cb (GtkCheckMenuItem *checkmenuitem, gpointer user_data);
void on_zoom_in_cb (GtkCheckMenuItem *checkmenuitem, gpointer user_data);
void on_zoom_out_cb (GtkCheckMenuItem *checkmenuitem, gpointer user_data);
void on_chart_snapshot_cb (GtkCheckMenuItem *checkmenuitem, gpointer user_data);

#endif /* __BAOBAB_CALLBACKS_H__ */
