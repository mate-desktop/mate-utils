/* screenshot-dialog.c - main MATE Screenshot dialog
 *
 * Copyright (C) 2001-2006  Jonathan Blandford <jrb@alum.mit.edu>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 */

#include <config.h>
#include <string.h>
#include <stdlib.h>

#include "screenshot-dialog.h"
#include "screenshot-save.h"
#include <gdk/gdkkeysyms.h>
#include <glib/gi18n.h>
#include <gio/gio.h>

enum {
  TYPE_IMAGE_PNG,
  TYPE_TEXT_URI_LIST,

  LAST_TYPE
};

static GtkTargetEntry drag_types[] =
{
  { "image/png", 0, TYPE_IMAGE_PNG },
  { "text/uri-list", 0, TYPE_TEXT_URI_LIST },
};

struct ScreenshotDialog
{
  GtkBuilder *ui;
  GdkPixbuf *screenshot;
  GdkPixbuf *preview_image;
  GtkWidget *save_widget;
  GtkWidget *filename_entry;
  gint drag_x;
  gint drag_y;
};

static gboolean
on_toplevel_key_press_event (GtkWidget *widget,
			     GdkEventKey *key)
{
  if (key->keyval == GDK_KEY_F1)
    {
      gtk_dialog_response (GTK_DIALOG (widget), GTK_RESPONSE_HELP);
      return TRUE;
    }

  return FALSE;
}

static void
on_preview_draw (GtkWidget      *drawing_area,
                 cairo_t        *cr,
                 gpointer        data)
{
  ScreenshotDialog *dialog = data;
  GtkStyleContext *context;
  int width, height;

  width = gtk_widget_get_allocated_width (drawing_area);
  height = gtk_widget_get_allocated_height (drawing_area);

  if (!dialog->preview_image ||
      gdk_pixbuf_get_width (dialog->preview_image) != width ||
      gdk_pixbuf_get_height (dialog->preview_image) != height)
    {
      g_clear_object (&dialog->preview_image);
      dialog->preview_image = gdk_pixbuf_scale_simple (dialog->screenshot,
                                                       width,
                                                       height,
                                                       GDK_INTERP_BILINEAR);
    }

  context = gtk_widget_get_style_context (drawing_area);
  gtk_style_context_save (context);

  gtk_style_context_set_state (context, gtk_widget_get_state_flags (drawing_area));
  gtk_render_icon (context, cr, dialog->preview_image, 0, 0);

  gtk_style_context_restore (context);
}

static gboolean
on_preview_button_press_event (GtkWidget      *drawing_area,
			       GdkEventButton *event,
			       gpointer        data)
{
  ScreenshotDialog *dialog = data;

  dialog->drag_x = (int) event->x;
  dialog->drag_y = (int) event->y;

  return FALSE;
}

static gboolean
on_preview_button_release_event (GtkWidget      *drawing_area,
				 GdkEventButton *event,
				 gpointer        data)
{
  ScreenshotDialog *dialog = data;

  dialog->drag_x = 0;
  dialog->drag_y = 0;

  return FALSE;
}

static void
drag_data_get (GtkWidget          *widget,
	       GdkDragContext     *context,
	       GtkSelectionData   *selection_data,
	       guint               info,
	       guint               time,
	       ScreenshotDialog   *dialog)
{
  if (info == TYPE_TEXT_URI_LIST)
    {
      gchar **uris;

      uris = g_new (gchar *, 2);
      uris[0] = g_strconcat ("file://",
                             screenshot_save_get_filename (),
                             NULL);
      uris[1] = NULL;

      gtk_selection_data_set_uris (selection_data, uris);
      g_strfreev (uris);
    }
  else if (info == TYPE_IMAGE_PNG)
    {
      gtk_selection_data_set_pixbuf (selection_data, dialog->screenshot);
    }
  else
    {
      g_warning ("Unknown type %d", info);
    }
}

static void
drag_begin (GtkWidget        *widget,
	    GdkDragContext   *context,
	    ScreenshotDialog *dialog)
{
  gtk_drag_set_icon_pixbuf (context, dialog->preview_image,
			    dialog->drag_x, dialog->drag_y);
}


ScreenshotDialog *
screenshot_dialog_new (GdkPixbuf *screenshot,
		       char      *initial_uri,
		       gboolean   take_window_shot)
{
  ScreenshotDialog *dialog;
  GtkWidget *toplevel;
  GtkWidget *preview_darea;
  GtkWidget *aspect_frame;
  GtkWidget *file_chooser_box;
  gint width, height;
  char *current_folder;
  char *current_name;
  char *ext;
  gint pos;
  GFile *tmp_file;
  GFile *parent_file;
  GError *error = NULL;

  tmp_file = g_file_new_for_uri (initial_uri);
  parent_file = g_file_get_parent (tmp_file);

  current_name = g_file_get_basename (tmp_file);
  current_folder = g_file_get_uri (parent_file);
  g_object_unref (tmp_file);
  g_object_unref (parent_file);

  dialog = g_new0 (ScreenshotDialog, 1);

  dialog->ui = gtk_builder_new ();

  dialog->screenshot = screenshot;

  if (gtk_builder_add_from_resource (dialog->ui, "/org/mate/screenshot/mate-screenshot.ui", &error) == 0)
    {
      GtkWidget *dialog;
      dialog = gtk_message_dialog_new (NULL, 0,
				       GTK_MESSAGE_ERROR,
				       GTK_BUTTONS_OK,
				       _("Error loading UI definition file for the screenshot program: \n%s\n\n"
				         "Please check your installation of mate-utils."), error->message);
      gtk_dialog_run (GTK_DIALOG (dialog));
      gtk_widget_destroy (dialog);

      g_error_free (error);
      exit (1);
    }

  gtk_builder_set_translation_domain (dialog->ui, GETTEXT_PACKAGE);

  width = gdk_pixbuf_get_width (screenshot);
  height = gdk_pixbuf_get_height (screenshot);

  width /= 5;
  height /= 5;

  toplevel = GTK_WIDGET (gtk_builder_get_object (dialog->ui, "toplevel"));
  aspect_frame = GTK_WIDGET (gtk_builder_get_object (dialog->ui, "aspect_frame"));
  preview_darea = GTK_WIDGET (gtk_builder_get_object (dialog->ui, "preview_darea"));
  dialog->filename_entry = GTK_WIDGET (gtk_builder_get_object (dialog->ui, "filename_entry"));
  file_chooser_box = GTK_WIDGET (gtk_builder_get_object (dialog->ui, "file_chooser_box"));

  dialog->save_widget = gtk_file_chooser_button_new (_("Select a folder"), GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);
  gtk_file_chooser_set_local_only (GTK_FILE_CHOOSER (dialog->save_widget), FALSE);
  gtk_file_chooser_set_current_folder_uri (GTK_FILE_CHOOSER (dialog->save_widget), current_folder);
  gtk_entry_set_text (GTK_ENTRY (dialog->filename_entry), current_name);

  gtk_box_pack_start (GTK_BOX (file_chooser_box), dialog->save_widget, TRUE, TRUE, 0);
  g_free (current_folder);

  gtk_widget_set_size_request (preview_darea, width, height);
  gtk_aspect_frame_set (GTK_ASPECT_FRAME (aspect_frame), 0.0, 0.5,
			gdk_pixbuf_get_width (screenshot)/
			(gfloat) gdk_pixbuf_get_height (screenshot),
			FALSE);
  g_signal_connect (toplevel, "key_press_event", G_CALLBACK (on_toplevel_key_press_event), dialog);
  g_signal_connect (preview_darea, "draw", G_CALLBACK (on_preview_draw), dialog);
  g_signal_connect (preview_darea, "button_press_event", G_CALLBACK (on_preview_button_press_event), dialog);
  g_signal_connect (preview_darea, "button_release_event", G_CALLBACK (on_preview_button_release_event), dialog);

  if (take_window_shot)
    gtk_frame_set_shadow_type (GTK_FRAME (aspect_frame), GTK_SHADOW_NONE);
  else
    gtk_frame_set_shadow_type (GTK_FRAME (aspect_frame), GTK_SHADOW_IN);

  /* setup dnd */
  g_signal_connect (G_OBJECT (preview_darea), "drag_begin",
		    G_CALLBACK (drag_begin), dialog);
  g_signal_connect (G_OBJECT (preview_darea), "drag_data_get",
		    G_CALLBACK (drag_data_get), dialog);

  gtk_widget_show_all (toplevel);

  /* select the name of the file but leave out the extension if there's any;
   * the dialog must be realized for select_region to work
   */
  ext = g_utf8_strrchr (current_name, -1, '.');
  if (ext)
    pos = g_utf8_strlen (current_name, -1) - g_utf8_strlen (ext, -1);
  else
    pos = -1;

  gtk_editable_select_region (GTK_EDITABLE (dialog->filename_entry),
			      0,
			      pos);

  g_free (current_name);

  return dialog;
}

void
screenshot_dialog_focus_entry (ScreenshotDialog *dialog)
{
  gtk_widget_grab_focus (dialog->filename_entry);
}

void
screenshot_dialog_enable_dnd (ScreenshotDialog *dialog)
{
  GtkWidget *preview_darea;

  g_return_if_fail (dialog != NULL);

  preview_darea = GTK_WIDGET (gtk_builder_get_object (dialog->ui, "preview_darea"));
  gtk_drag_source_set (preview_darea,
		       GDK_BUTTON1_MASK | GDK_BUTTON3_MASK,
		       drag_types, G_N_ELEMENTS (drag_types),
		       GDK_ACTION_COPY);
}

GtkWidget *
screenshot_dialog_get_toplevel (ScreenshotDialog *dialog)
{
  return GTK_WIDGET (gtk_builder_get_object (dialog->ui, "toplevel"));
}

char *
screenshot_dialog_get_uri (ScreenshotDialog *dialog)
{
  gchar *folder;
  const gchar *file_name;
  gchar *uri, *file, *tmp;
  GError *error;

  folder = gtk_file_chooser_get_uri (GTK_FILE_CHOOSER (dialog->save_widget));
  file_name = gtk_entry_get_text (GTK_ENTRY (dialog->filename_entry));

  error = NULL;
  tmp = g_filename_from_utf8 (file_name, -1, NULL, NULL, &error);
  if (error)
    {
      g_warning ("Unable to convert `%s' to valid UTF-8: %s\n"
                 "Falling back to default file.",
                 file_name,
                 error->message);
      g_error_free (error);
      tmp = g_strdup (_("Screenshot.png"));
    }

  file = g_uri_escape_string (tmp, NULL, FALSE);
  uri = g_build_filename (folder, file, NULL);

  g_free (folder);
  g_free (tmp);
  g_free (file);

  return uri;
}

char *
screenshot_dialog_get_folder (ScreenshotDialog *dialog)
{
  return gtk_file_chooser_get_uri (GTK_FILE_CHOOSER (dialog->save_widget));
}

GdkPixbuf *
screenshot_dialog_get_screenshot (ScreenshotDialog *dialog)
{
  return dialog->screenshot;
}

void
screenshot_dialog_set_busy (ScreenshotDialog *dialog,
			    gboolean          busy)
{
  GtkWidget *toplevel;
  GdkDisplay *display;

  toplevel = screenshot_dialog_get_toplevel (dialog);
  display = gtk_widget_get_display (toplevel);

  if (busy)
    {
      GdkCursor *cursor;
      /* Change cursor to busy */
      cursor = gdk_cursor_new_for_display (display, GDK_WATCH);
      gdk_window_set_cursor (gtk_widget_get_window (toplevel), cursor);
      g_object_unref (cursor);
    }
  else
    {
      gdk_window_set_cursor (gtk_widget_get_window (toplevel), NULL);
    }

  gtk_widget_set_sensitive (toplevel, ! busy);

  gdk_display_flush (display);
}
