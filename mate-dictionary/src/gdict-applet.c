/* gdict-applet.c - MATE Dictionary Applet
 *
 * Copyright (c) 2005  Emmanuele Bassi <ebassi@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,  
 * but WITHOUT ANY WARRANTY; without even the implied warranty of  
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the  
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License  
 * along with this program; if not, write to the Free Software  
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib/gi18n.h>
#include <gio/gio.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "gdict-applet.h"
#include "gdict-about.h"
#include "gdict-pref-dialog.h"
#include "gdict-print.h"
#include "gdict-common.h"
#include "gdict-aligned-window.h"

#define GDICT_APPLET_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), GDICT_TYPE_APPLET, GdictAppletClass))
#define GDICT_APPLET_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), GDICT_TYPE_APPLET, GdictAppletClass))

struct _GdictAppletClass
{
  MatePanelAppletClass parent_class;
};

#define GDICT_APPLET_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE ((obj), GDICT_TYPE_APPLET, GdictAppletPrivate))

struct _GdictAppletPrivate
{
  guint size;
  GtkOrientation orient;
  
  GSettings *settings;
  GSettings *desktop_settings;

  gchar *database;
  gchar *strategy;
  gchar *source_name;  
  gchar *print_font;
  gchar *defbox_font;

  gchar *word;  
  GdictContext *context;
  guint lookup_start_id;
  guint lookup_end_id;
  guint error_id;

  GdictSourceLoader *loader;

  GtkWidget *box;
  GtkWidget *toggle;
  GtkWidget *image;
  GtkWidget *entry;
  GtkWidget *window;
  GtkWidget *frame;
  GtkWidget *defbox;

  GtkActionGroup* context_menu_action_group;

  guint idle_draw_id;

  GdkPixbuf *icon;

  gint window_width;
  gint window_height;

  guint is_window_showing : 1;
};

#define WINDOW_MIN_WIDTH 	300
#define WINDOW_MIN_HEIGHT 	200
#define WINDOW_NUM_COLUMNS 	47
#define WINDOW_NUM_ROWS  	20

G_DEFINE_TYPE (GdictApplet, gdict_applet, PANEL_TYPE_APPLET);


static const GtkTargetEntry drop_types[] =
{
  { "text/plain",    0, 0 },
  { "TEXT",          0, 0 },
  { "STRING",        0, 0 },
  { "UTF8_STRING",   0, 0 },
};
static const guint n_drop_types = G_N_ELEMENTS (drop_types);


static void
set_atk_name_description (GtkWidget  *widget,
			  const char *name,
			  const char *description)
{	
  AtkObject *aobj;
	
  aobj = gtk_widget_get_accessible (widget);
  if (!GTK_IS_ACCESSIBLE (aobj))
    return;

  atk_object_set_name (aobj, name);
  atk_object_set_description (aobj, description);
}

static void
set_window_default_size (GdictApplet *applet)
{
  GdictAppletPrivate *priv = applet->priv;
  GtkWidget *widget, *defbox;
  gint width, height;
  gint font_size;
  GdkScreen *screen;
  gint monitor_num;
  GtkRequisition req;
  GdkRectangle monitor;

  if (!priv->window)
    return;
  
  widget = priv->window;
  defbox = priv->defbox;

  /* Size based on the font size */
  font_size = pango_font_description_get_size (gtk_widget_get_style (defbox)->font_desc);
  font_size = PANGO_PIXELS (font_size);

  width = font_size * WINDOW_NUM_COLUMNS;
  height = font_size * WINDOW_NUM_ROWS;

  /* Use at least the requisition size of the window... */
  gtk_widget_get_preferred_size (widget, NULL, &req);
  width = MAX (width, req.width);
  height = MAX (height, req.height);

  /* ... but make it no larger than half the monitor size */
  screen = gtk_widget_get_screen (widget);
  monitor_num = gdk_screen_get_monitor_at_window (screen,
                                                  gtk_widget_get_window (widget));

  gdk_screen_get_monitor_geometry (screen, monitor_num, &monitor);

  width = MIN (width, monitor.width / 2);
  height = MIN (height, monitor.height / 2);

  /* Set size */
  gtk_widget_set_size_request (priv->frame, width, height);
}

static void
clear_cb (GtkWidget   *widget,
	  GdictApplet *applet)
{
  GdictAppletPrivate *priv = applet->priv;

  gtk_entry_set_text (GTK_ENTRY (priv->entry), "");

  if (!priv->defbox)
    return;
  
  gdict_defbox_clear (GDICT_DEFBOX (priv->defbox));
}

static void
print_cb (GtkWidget   *widget,
	  GdictApplet *applet)
{
  GdictAppletPrivate *priv = applet->priv;

  if (!priv->defbox)
    return;
  
  gdict_show_print_dialog (GTK_WINDOW (priv->window),
  			   GDICT_DEFBOX (priv->defbox));
}

static void
save_cb (GtkWidget   *widget,
         GdictApplet *applet)
{
  GdictAppletPrivate *priv = applet->priv;
  GtkWidget *dialog;

  if (!priv->defbox)
    return;
  
  dialog = gtk_file_chooser_dialog_new (_("Save a Copy"),
  					GTK_WINDOW (priv->window),
  					GTK_FILE_CHOOSER_ACTION_SAVE,
  					GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
  					GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
  					NULL);
  gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);
  
  /* default to user's $HOME */
  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), g_get_home_dir ());
  gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (dialog), _("Untitled document"));
  
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
      gchar *filename;
      gchar *text;
      gsize len;
      GError *write_error = NULL;
      
      filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
      
      text = gdict_defbox_get_text (GDICT_DEFBOX (priv->defbox), &len);
      
      g_file_set_contents (filename,
      			   text,
      			   len,
      			   &write_error);
      if (write_error)
        {
          gchar *message;
          
          message = g_strdup_printf (_("Error while writing to '%s'"), filename);
          
          gdict_show_error_dialog (GTK_WINDOW (priv->window),
          			   message,
          			   write_error->message);
          
          g_error_free (write_error);
          g_free (message);
        }
      
      g_free (text);
      g_free (filename);
    }

  gtk_widget_destroy (dialog);
}

static void
gdict_applet_set_menu_items_sensitive (GdictApplet *applet,
				       gboolean     is_sensitive)
{
  GtkAction *action;

  action = gtk_action_group_get_action (applet->priv->context_menu_action_group,
                                        "DictionaryClear");
  gtk_action_set_sensitive (action, is_sensitive);
  action = gtk_action_group_get_action (applet->priv->context_menu_action_group,
                                        "DictionaryPrint");
  gtk_action_set_sensitive (action, is_sensitive);
  action = gtk_action_group_get_action (applet->priv->context_menu_action_group,
                                        "DictionarySave");
  gtk_action_set_sensitive (action, is_sensitive);

	return;
}

static gboolean
window_key_press_event_cb (GtkWidget   *widget,
			   GdkEventKey *event,
			   gpointer     user_data)
{
  GdictApplet *applet = GDICT_APPLET (user_data);
  
  if (event->keyval == GDK_KEY_Escape)
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (applet->priv->toggle), FALSE);
      gtk_toggle_button_toggled (GTK_TOGGLE_BUTTON (applet->priv->toggle));

      return TRUE;
    }
  else if ((event->keyval == GDK_KEY_l) &&
	   (event->state & GDK_CONTROL_MASK))
    {
      gtk_widget_grab_focus (applet->priv->entry);

      return TRUE;
    }

  return FALSE;
}

static void
window_show_cb (GtkWidget   *window,
		GdictApplet *applet)
{
  set_window_default_size (applet);
}

static void
gdict_applet_build_window (GdictApplet *applet)
{
  GdictAppletPrivate *priv = applet->priv;
  GtkWidget *window;
  GtkWidget *frame;
  GtkWidget *vbox;
  GtkWidget *bbox;
  GtkWidget *button;

  if (!priv->entry)
    {
      g_warning ("No entry widget defined");

      return;
    }
  
  window = gdict_aligned_window_new (priv->toggle);
  g_signal_connect (window, "key-press-event",
		    G_CALLBACK (window_key_press_event_cb),
		    applet);
  g_signal_connect (window, "show",
  		    G_CALLBACK (window_show_cb),
		    applet);

  frame = gtk_frame_new (NULL);
  gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
  gtk_container_add (GTK_CONTAINER (window), frame);
  gtk_widget_show (frame);
  priv->frame = frame;

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 12);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 6);
  gtk_container_add (GTK_CONTAINER (frame), vbox);
  gtk_widget_show (vbox);
  
  priv->defbox = gdict_defbox_new ();
  if (priv->context)
    gdict_defbox_set_context (GDICT_DEFBOX (priv->defbox), priv->context);
  
  gtk_box_pack_start (GTK_BOX (vbox), priv->defbox, TRUE, TRUE, 0);
  gtk_widget_show (priv->defbox);
  gtk_widget_set_can_focus (priv->defbox, TRUE);
  gtk_widget_set_can_default (priv->defbox, TRUE);

  bbox = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (bbox), GTK_BUTTONBOX_END);
  gtk_box_set_spacing (GTK_BOX (bbox), 6);
  gtk_box_pack_end (GTK_BOX (vbox), bbox, FALSE, FALSE, 0);
  gtk_widget_show (bbox);
  
  button = gtk_button_new_from_stock (GTK_STOCK_CLEAR);
  gtk_widget_set_tooltip_text (button, _("Clear the definitions found"));
  set_atk_name_description (button,
		  	    _("Clear definition"),
			    _("Clear the text of the definition"));
  
  g_signal_connect (button, "clicked", G_CALLBACK (clear_cb), applet);
  gtk_box_pack_start (GTK_BOX (bbox), button, FALSE, FALSE, 0);
  gtk_widget_show (button);

  button = gtk_button_new_from_stock (GTK_STOCK_PRINT);
  gtk_widget_set_tooltip_text (button, _("Print the definitions found"));
  set_atk_name_description (button,
		  	    _("Print definition"),
			    _("Print the text of the definition"));
  
  g_signal_connect (button, "clicked", G_CALLBACK (print_cb), applet);
  gtk_box_pack_start (GTK_BOX (bbox), button, FALSE, FALSE, 0);
  gtk_widget_show (button);

  button = gtk_button_new_from_stock (GTK_STOCK_SAVE);
  gtk_widget_set_tooltip_text (button, _("Save the definitions found"));
  set_atk_name_description (button,
		  	    _("Save definition"),
			    _("Save the text of the definition to a file"));
  
  g_signal_connect (button, "clicked", G_CALLBACK (save_cb), applet);
  gtk_box_pack_start (GTK_BOX (bbox), button, FALSE, FALSE, 0);
  gtk_widget_show (button);
  
  gtk_window_set_default (GTK_WINDOW (window), priv->defbox);
  
  priv->window = window;
  priv->is_window_showing = FALSE;
}

static gboolean
gdict_applet_icon_toggled_cb (GtkWidget   *widget,
			      GdictApplet *applet)
{
  GdictAppletPrivate *priv = applet->priv;

  if (!priv->window)
    gdict_applet_build_window (applet);
  
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
    {
      gtk_window_set_screen (GTK_WINDOW (priv->window),
	                     gtk_widget_get_screen (GTK_WIDGET (applet)));
      gtk_window_present (GTK_WINDOW (priv->window));
      gtk_widget_grab_focus (priv->defbox);
      
      priv->is_window_showing = TRUE;
    }
  else
    {
      /* force hiding the find pane */
      gdict_defbox_set_show_find (GDICT_DEFBOX (priv->defbox), FALSE);

      gtk_widget_grab_focus (priv->entry);
      gtk_widget_hide (priv->window);
      
      priv->is_window_showing = FALSE;
    }

  return FALSE;
}

static void
gdict_applet_entry_activate_cb (GtkWidget   *widget,
				GdictApplet *applet)
{
  GdictAppletPrivate *priv = applet->priv;
  gchar *text;
  
  text = gtk_editable_get_chars (GTK_EDITABLE (widget), 0, -1);
  if (!text)
    return;
  
  g_free (priv->word);
  priv->word = text;
  
  if (!priv->window)
    gdict_applet_build_window (applet);

  gdict_defbox_lookup (GDICT_DEFBOX (priv->defbox), priv->word);
}

static gboolean
gdict_applet_entry_key_press_cb (GtkWidget   *widget,
				 GdkEventKey *event,
				 gpointer     user_data)
{
  GdictAppletPrivate *priv = GDICT_APPLET (user_data)->priv;
  
  if (event->keyval == GDK_KEY_Escape)
    {
      if (priv->is_window_showing)
	{
          gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (priv->toggle), FALSE);
	  gtk_toggle_button_toggled (GTK_TOGGLE_BUTTON (priv->toggle));
	  
	  return TRUE;
	}
    }
  else if (event->keyval == GDK_KEY_Tab)
    {
      if (priv->is_window_showing)
	gtk_widget_grab_focus (priv->defbox);
    }

  return FALSE;
}

static gboolean
gdict_applet_icon_button_press_event_cb (GtkWidget      *widget,
					 GdkEventButton *event,
					 GdictApplet    *applet)
{
  GdictAppletPrivate *priv = applet->priv;
  
  /* we don't want to block the applet's popup menu unless the
   * user is toggling the button
   */
  if (event->button != 1)
    g_signal_stop_emission_by_name (priv->toggle, "button-press-event");

  return FALSE;
}

static gboolean
gdict_applet_entry_button_press_event_cb (GtkWidget      *widget,
					  GdkEventButton *event,
					  GdictApplet    *applet)
{
  mate_panel_applet_request_focus (MATE_PANEL_APPLET (applet), event->time);

  return FALSE;
}

static gboolean
gdict_applet_draw (GdictApplet *applet)
{
  GdictAppletPrivate *priv = applet->priv;
  GtkWidget *box;
  GtkWidget *hbox;
  gchar *text = NULL;

  if (priv->entry)
    text = gtk_editable_get_chars (GTK_EDITABLE (priv->entry), 0, -1);
  
  if (priv->box)
    gtk_widget_destroy (priv->box);

  box = gtk_box_new (priv->orient, 0);
  
  gtk_container_add (GTK_CONTAINER (applet), box);
  gtk_widget_show (box);

  /* toggle button */
  priv->toggle = gtk_toggle_button_new ();
  gtk_widget_set_tooltip_text (priv->toggle, _("Click to view the dictionary window"));
  set_atk_name_description (priv->toggle,
			    _("Toggle dictionary window"),
		  	    _("Show or hide the definition window"));
  
  gtk_button_set_relief (GTK_BUTTON (priv->toggle),
		  	 GTK_RELIEF_NONE);
  g_signal_connect (priv->toggle, "toggled",
		    G_CALLBACK (gdict_applet_icon_toggled_cb),
		    applet);
  g_signal_connect (priv->toggle, "button-press-event",
		    G_CALLBACK (gdict_applet_icon_button_press_event_cb),
		    applet);
  gtk_box_pack_start (GTK_BOX (box), priv->toggle, FALSE, FALSE, 0);
  gtk_widget_show (priv->toggle);

  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_container_set_border_width (GTK_CONTAINER (hbox), 0);
  gtk_container_add (GTK_CONTAINER (priv->toggle), hbox);
  gtk_widget_show (hbox);

  if (priv->icon)
    {
      GdkPixbuf *scaled;
      
      priv->image = gtk_image_new ();
      gtk_image_set_pixel_size (GTK_IMAGE (priv->image), priv->size - 10);

      scaled = gdk_pixbuf_scale_simple (priv->icon,
		      			priv->size - 5,
					priv->size - 5,
					GDK_INTERP_BILINEAR);
      
      gtk_image_set_from_pixbuf (GTK_IMAGE (priv->image), scaled);
      g_object_unref (scaled);
      
      gtk_box_pack_start (GTK_BOX (hbox), priv->image, FALSE, FALSE, 0);
      
      gtk_widget_show (priv->image);
    }
  else
    {
      priv->image = gtk_image_new ();

      gtk_image_set_pixel_size (GTK_IMAGE (priv->image), priv->size - 10);
      gtk_image_set_from_icon_name (GTK_IMAGE (priv->image),
                                    "image-missing", -1);
      
      gtk_box_pack_start (GTK_BOX (hbox), priv->image, FALSE, FALSE, 0);
      gtk_widget_show (priv->image);
    }

  /* entry */
  priv->entry = gtk_entry_new ();
  gtk_widget_set_tooltip_text (priv->entry, _("Type the word you want to look up"));
  set_atk_name_description (priv->entry,
		  	    _("Dictionary entry"),
			    _("Look up words in dictionaries"));
  
  gtk_editable_set_editable (GTK_EDITABLE (priv->entry), TRUE);
  gtk_entry_set_width_chars (GTK_ENTRY (priv->entry), 12);
  g_signal_connect (priv->entry, "activate",
  		    G_CALLBACK (gdict_applet_entry_activate_cb),
  		    applet);
  g_signal_connect (priv->entry, "button-press-event",
		    G_CALLBACK (gdict_applet_entry_button_press_event_cb),
		    applet);
  g_signal_connect (priv->entry, "key-press-event",
		    G_CALLBACK (gdict_applet_entry_key_press_cb),
		    applet);
  gtk_box_pack_end (GTK_BOX (box), priv->entry, FALSE, FALSE, 0);
  gtk_widget_show (priv->entry);

  if (text)
    {
      gtk_entry_set_text (GTK_ENTRY (priv->entry), text);

      g_free (text);
    }
  
  priv->box = box;

#if 0
  gtk_widget_grab_focus (priv->entry);
#endif
  
  gtk_widget_show_all (GTK_WIDGET (applet));

  return FALSE;
}

static void
gdict_applet_queue_draw (GdictApplet *applet)
{
  if (!applet->priv->idle_draw_id)
    applet->priv->idle_draw_id = g_idle_add ((GSourceFunc) gdict_applet_draw,
					     applet);
}

static void
gdict_applet_lookup_start_cb (GdictContext *context,
			      GdictApplet  *applet)
{
  GdictAppletPrivate *priv = applet->priv;

  if (!priv->window)
    gdict_applet_build_window (applet);

  if (!priv->is_window_showing)
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (priv->toggle), TRUE);
      
      gtk_window_present (GTK_WINDOW (priv->window));
      gtk_widget_grab_focus (priv->defbox);

      priv->is_window_showing = TRUE;
    }

  gdict_applet_set_menu_items_sensitive (applet, FALSE);
}

static void
gdict_applet_lookup_end_cb (GdictContext *context,
			    GdictApplet  *applet)
{
  gdict_applet_set_menu_items_sensitive (applet, TRUE);

  gtk_window_present (GTK_WINDOW (applet->priv->window));
}

static void
gdict_applet_error_cb (GdictContext *context,
		       const GError *error,
		       GdictApplet  *applet)
{
  /* disable menu items */
  gdict_applet_set_menu_items_sensitive (applet, FALSE);
}

static void
gdict_applet_cmd_lookup (GtkAction *action,
			 GdictApplet       *applet)
{
  GdictAppletPrivate *priv = applet->priv;
  gchar *text = NULL;;
  
  text = gtk_editable_get_chars (GTK_EDITABLE (priv->entry), 0, -1);
  if (!text)
    return;
  
  g_free (priv->word);
  priv->word = text;
  
  if (!priv->window)
    gdict_applet_build_window (applet);
  
  gdict_defbox_lookup (GDICT_DEFBOX (priv->defbox), priv->word);
}

static void
gdict_applet_cmd_clear (GtkAction *action,
			GdictApplet       *applet)
{
  clear_cb (NULL, applet);
}

static void
gdict_applet_cmd_print (GtkAction *action,
			GdictApplet       *applet)
{
  print_cb (NULL, applet);
}

static void
gdict_applet_cmd_save (GtkAction *action,
			GdictApplet       *applet)
{
  save_cb (NULL, applet);
}

static void
gdict_applet_cmd_preferences (GtkAction *action,
			      GdictApplet       *applet)
{
  gdict_show_pref_dialog (GTK_WIDGET (applet),
  			  _("Dictionary Preferences"),
  			  applet->priv->loader);
}

static void
gdict_applet_cmd_about (GtkAction *action,
			GdictApplet       *applet)
{
  gdict_show_about_dialog (GTK_WIDGET (applet));
}

static void
gdict_applet_cmd_help (GtkAction *action,
		       GdictApplet       *applet)
{
  GError *err = NULL;

  gtk_show_uri (gtk_widget_get_screen (GTK_WIDGET (applet)),
		"help:mate-dictionary/mate-dictionary-applet",
		gtk_get_current_event_time (), &err);
  
  if (err)
    {
      gdict_show_error_dialog (NULL,
      			       _("There was an error while displaying help"),
      			       err->message);
      g_error_free (err);
    }
}

static void
gdict_applet_change_orient (MatePanelApplet       *applet,
			    MatePanelAppletOrient  orient)
{
  GdictAppletPrivate *priv = GDICT_APPLET (applet)->priv;
  guint new_size;
  GtkAllocation allocation;
  
  gtk_widget_get_allocation (GTK_WIDGET (applet), &allocation);
  switch (orient)
    {
    case MATE_PANEL_APPLET_ORIENT_LEFT:
    case MATE_PANEL_APPLET_ORIENT_RIGHT:
      priv->orient = GTK_ORIENTATION_VERTICAL;
      new_size = allocation.width;
      break;
    case MATE_PANEL_APPLET_ORIENT_UP:
    case MATE_PANEL_APPLET_ORIENT_DOWN:
      priv->orient = GTK_ORIENTATION_HORIZONTAL;
      new_size = allocation.height;
      break;
    }
  
  if (new_size != priv->size)
    priv->size = new_size;
  
  gdict_applet_queue_draw (GDICT_APPLET (applet));
  
  if (MATE_PANEL_APPLET_CLASS (gdict_applet_parent_class)->change_orient)
    MATE_PANEL_APPLET_CLASS (gdict_applet_parent_class)->change_orient (applet,
    								   orient);
}

static void
gdict_applet_size_allocate (GtkWidget    *widget,
			    GdkRectangle *allocation)
{
  GdictApplet *applet = GDICT_APPLET (widget);
  GdictAppletPrivate *priv = applet->priv;
  guint new_size;
 
  if (priv->orient == GTK_ORIENTATION_HORIZONTAL)
    new_size = allocation->height;
  else
    new_size = allocation->width;
  
  if (priv->size != new_size)
    {
      priv->size = new_size;

      gtk_image_set_pixel_size (GTK_IMAGE (priv->image), priv->size - 10);

      /* re-scale the icon, if it was found */
      if (priv->icon)
        {
          GdkPixbuf *scaled;

	  scaled = gdk_pixbuf_scale_simple (priv->icon,
			  		    priv->size - 5,
					    priv->size - 5,
					    GDK_INTERP_BILINEAR);
	  
	  gtk_image_set_from_pixbuf (GTK_IMAGE (priv->image), scaled);
	  g_object_unref (scaled);
	}
     }

  if (GTK_WIDGET_CLASS (gdict_applet_parent_class)->size_allocate)
    GTK_WIDGET_CLASS (gdict_applet_parent_class)->size_allocate (widget,
		    						 allocation);
}

static void
gdict_applet_style_set (GtkWidget *widget,
			GtkStyle  *old_style)
{
  if (GTK_WIDGET_CLASS (gdict_applet_parent_class)->style_set)
    GTK_WIDGET_CLASS (gdict_applet_parent_class)->style_set (widget,
		    					     old_style);
#if 0
  set_window_default_size (GDICT_APPLET (widget));
#endif
}

static void
gdict_applet_set_database (GdictApplet *applet,
			   const gchar *database)
{
  GdictAppletPrivate *priv = applet->priv;
  
  g_free (priv->database);

  if (database != NULL && *database != '\0')
    priv->database = g_strdup (database);
  else
    priv->database = g_settings_get_string (priv->settings,
							  GDICT_SETTINGS_DATABASE_KEY);
  if (priv->defbox)
    gdict_defbox_set_database (GDICT_DEFBOX (priv->defbox),
			       priv->database);
}

static void
gdict_applet_set_strategy (GdictApplet *applet,
			   const gchar *strategy)
{
  GdictAppletPrivate *priv = applet->priv;
  
  g_free (priv->strategy);

  if (strategy != NULL && *strategy != '\0')
    priv->strategy = g_strdup (strategy);
  else
    priv->strategy = g_settings_get_string (priv->settings,
							  GDICT_SETTINGS_STRATEGY_KEY);
}

static GdictContext *
get_context_from_loader (GdictApplet *applet)
{
  GdictAppletPrivate *priv = applet->priv;
  GdictSource *source;
  GdictContext *retval;

  if (!priv->source_name)
    priv->source_name = g_strdup (GDICT_DEFAULT_SOURCE_NAME);

  source = gdict_source_loader_get_source (priv->loader,
		  			   priv->source_name);
  if (!source)
    {
      gchar *detail;
      
      detail = g_strdup_printf (_("No dictionary source available with name '%s'"),
      				priv->source_name);

      gdict_show_error_dialog (NULL,
                               _("Unable to find dictionary source"),
                               NULL);
      
      g_free (detail);

      return NULL;
    }
  
  gdict_applet_set_database (applet, gdict_source_get_database (source));
  gdict_applet_set_strategy (applet, gdict_source_get_strategy (source));
  
  retval = gdict_source_get_context (source);
  if (!retval)
    {
      gchar *detail;
      
      detail = g_strdup_printf (_("No context available for source '%s'"),
      				gdict_source_get_description (source));
      				
      gdict_show_error_dialog (NULL,
                               _("Unable to create a context"),
                               detail);
      
      g_free (detail);
      g_object_unref (source);
      
      return NULL;
    }
  
  g_object_unref (source);
  
  return retval;
}

static void
gdict_applet_set_print_font (GdictApplet *applet,
			     const gchar *print_font)
{
  GdictAppletPrivate *priv = applet->priv;

  g_free (priv->print_font);

  if (print_font != NULL && *print_font != '\0')
    priv->print_font = g_strdup (print_font);
  else
    priv->print_font = g_settings_get_string (priv->settings,
							    GDICT_SETTINGS_PRINT_FONT_KEY);
}

static void
gdict_applet_set_defbox_font (GdictApplet *applet,
			      const gchar *defbox_font)
{
  GdictAppletPrivate *priv = applet->priv;

  g_free (priv->defbox_font);

  if (defbox_font != NULL && *defbox_font != '\0')
    priv->defbox_font = g_strdup (defbox_font);
  else
    priv->defbox_font = g_settings_get_string (priv->desktop_settings,
							     DOCUMENT_FONT_KEY);

  if (priv->defbox)
    gdict_defbox_set_font_name (GDICT_DEFBOX (priv->defbox),
				priv->defbox_font);
}

static void
gdict_applet_set_context (GdictApplet  *applet,
			  GdictContext *context)
{
  GdictAppletPrivate *priv = applet->priv;
  
  if (priv->context)
    {
      g_signal_handler_disconnect (priv->context, priv->lookup_start_id);
      g_signal_handler_disconnect (priv->context, priv->lookup_end_id);
      g_signal_handler_disconnect (priv->context, priv->error_id);
      
      priv->lookup_start_id = 0;
      priv->lookup_end_id = 0;
      priv->error_id = 0;
      
      g_object_unref (priv->context);
      priv->context = NULL;
    }

  if (priv->defbox)
    gdict_defbox_set_context (GDICT_DEFBOX (priv->defbox), context);

  if (!context)
    return;
  
  /* attach our callbacks */
  priv->lookup_start_id = g_signal_connect (context, "lookup-start",
					    G_CALLBACK (gdict_applet_lookup_start_cb),
					    applet);
  priv->lookup_end_id   = g_signal_connect (context, "lookup-end",
					    G_CALLBACK (gdict_applet_lookup_end_cb),
					    applet);
  priv->error_id        = g_signal_connect (context, "error",
		  			    G_CALLBACK (gdict_applet_error_cb),
					    applet);
  
  priv->context = context;
}

static void
gdict_applet_set_source_name (GdictApplet *applet,
			      const gchar *source_name)
{
  GdictAppletPrivate *priv = applet->priv;
  GdictContext *context;

  g_free (priv->source_name);

  if (source_name != NULL && *source_name != '\0')
    priv->source_name = g_strdup (source_name);
  else
    priv->source_name = g_settings_get_string (priv->settings,
							     GDICT_SETTINGS_SOURCE_KEY);
  
  context = get_context_from_loader (applet);
  gdict_applet_set_context (applet, context);
}

static void
gdict_applet_settings_changed_cb (GSettings *settings,
                  const gchar *key, GdictApplet *applet)
{

  if (g_strcmp0 (key, GDICT_SETTINGS_PRINT_FONT_KEY) == 0)
    {
        gdict_applet_set_print_font (applet, NULL);
    }
  else if (g_strcmp0 (key, GDICT_SETTINGS_SOURCE_KEY) == 0)
    {
        gdict_applet_set_source_name (applet, NULL);
    }
  else if (g_strcmp0 (key, GDICT_SETTINGS_DATABASE_KEY) == 0)
    {
        gdict_applet_set_database (applet, NULL);
    }
  else if (g_strcmp0 (key, GDICT_SETTINGS_STRATEGY_KEY) == 0)
    {
        gdict_applet_set_strategy (applet, NULL);
    }
  else if (g_strcmp0 (key, DOCUMENT_FONT_KEY) == 0)
    {
        gdict_applet_set_defbox_font (applet, NULL);
    }
}

static void
gdict_applet_finalize (GObject *object)
{
  GdictApplet *applet = GDICT_APPLET (object);
  GdictAppletPrivate *priv = applet->priv;

  if (priv->idle_draw_id)
    g_source_remove (priv->idle_draw_id);

  if (priv->context_menu_action_group)
    {
      g_object_unref (priv->context_menu_action_group);
      priv->context_menu_action_group = NULL;
    }

  if (priv->settings != NULL)
    {
      g_object_unref (priv->settings);
      priv->settings = NULL;
    }

  if (priv->desktop_settings != NULL)
    {
      g_object_unref (priv->desktop_settings);
      priv->desktop_settings = NULL;
    }

  if (priv->context)
    {
      if (priv->lookup_start_id)
        {
          g_signal_handler_disconnect (priv->context, priv->lookup_start_id);
          g_signal_handler_disconnect (priv->context, priv->lookup_end_id);
          g_signal_handler_disconnect (priv->context, priv->error_id);
        }

      g_object_unref (priv->context);
    }
  
  if (priv->loader)
    g_object_unref (priv->loader);
  
  if (priv->icon)
    g_object_unref (priv->icon);
  
  g_free (priv->source_name);
  g_free (priv->print_font);
  g_free (priv->defbox_font);
  g_free (priv->word);
  
  G_OBJECT_CLASS (gdict_applet_parent_class)->finalize (object);
}

static void
gdict_applet_class_init (GdictAppletClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  MatePanelAppletClass *applet_class = MATE_PANEL_APPLET_CLASS (klass);
  
  gobject_class->finalize = gdict_applet_finalize;
  
  widget_class->size_allocate = gdict_applet_size_allocate;
  widget_class->style_set = gdict_applet_style_set;
  
  applet_class->change_orient = gdict_applet_change_orient;
  
  g_type_class_add_private (gobject_class, sizeof (GdictAppletPrivate));
}

static void
gdict_applet_init (GdictApplet *applet)
{
  GdictAppletPrivate *priv;
  gchar *data_dir;

  priv = GDICT_APPLET_GET_PRIVATE (applet);
  applet->priv = priv;
      
  if (!priv->loader)
    priv->loader = gdict_source_loader_new ();

  /* add our data dir inside $HOME to the loader's search paths */
  data_dir = gdict_get_data_dir ();
  gdict_source_loader_add_search_path (priv->loader, data_dir);
  g_free (data_dir);
  
  gtk_window_set_default_icon_name ("accessories-dictionary");
  
  mate_panel_applet_set_flags (MATE_PANEL_APPLET (applet),
			  MATE_PANEL_APPLET_EXPAND_MINOR);

  priv->settings = g_settings_new (GDICT_SETTINGS_SCHEMA);
  priv->desktop_settings = g_settings_new (DESKTOP_SETTINGS_SCHEMA);

  g_signal_connect (priv->settings, "changed",
                    G_CALLBACK (gdict_applet_settings_changed_cb), applet);

  g_signal_connect (priv->desktop_settings, "changed",
                    G_CALLBACK (gdict_applet_settings_changed_cb), applet);

  mate_panel_applet_set_background_widget (MATE_PANEL_APPLET (applet),
		  		      GTK_WIDGET (applet));

  priv->size = mate_panel_applet_get_size (MATE_PANEL_APPLET (applet));

  switch (mate_panel_applet_get_orient (MATE_PANEL_APPLET (applet)))
    {
    case MATE_PANEL_APPLET_ORIENT_LEFT:
    case MATE_PANEL_APPLET_ORIENT_RIGHT:
      priv->orient = GTK_ORIENTATION_VERTICAL;
      break;
    case MATE_PANEL_APPLET_ORIENT_UP:
    case MATE_PANEL_APPLET_ORIENT_DOWN:
      priv->orient = GTK_ORIENTATION_HORIZONTAL;
      break;
    }

  priv->icon = gtk_icon_theme_load_icon (gtk_icon_theme_get_default (),
		  			 "accessories-dictionary",
					 48,
					 0,
					 NULL);
  
  /* force first draw */
  gdict_applet_draw (applet);

  /* force retrieval of the configuration from settings */
  gdict_applet_set_source_name (applet, NULL);
  gdict_applet_set_defbox_font (applet, NULL);
  gdict_applet_set_print_font (applet, NULL);
}

static const GtkActionEntry gdict_applet_menu_actions[] = {
  {"DictionaryLookup", GTK_STOCK_FIND, N_("_Look Up Selected Text"),
    NULL, NULL,
    G_CALLBACK (gdict_applet_cmd_lookup) },
  {"DictionaryClear", GTK_STOCK_CLEAR, N_("Cl_ear"),
    NULL, NULL,
    G_CALLBACK (gdict_applet_cmd_clear) },
  {"DictionaryPrint", GTK_STOCK_PRINT, N_("_Print"),
    NULL, NULL,
    G_CALLBACK (gdict_applet_cmd_print) },
  {"DictionarySave", GTK_STOCK_SAVE, N_("_Save"),
    NULL, NULL,
    G_CALLBACK (gdict_applet_cmd_save) },
  {"DictionaryPreferences", GTK_STOCK_PREFERENCES, N_("Preferences"),
    NULL, NULL,
    G_CALLBACK (gdict_applet_cmd_preferences) },
  {"DictionaryHelp", GTK_STOCK_HELP, N_("_Help"),
    NULL, NULL,
    G_CALLBACK (gdict_applet_cmd_help) },
  {"DictionaryAbout", GTK_STOCK_ABOUT, N_("_About"),
    NULL, NULL,
    G_CALLBACK (gdict_applet_cmd_about) },
};

static gboolean
gdict_applet_factory (MatePanelApplet *applet,
                      const gchar *iid,
		      gpointer     data)
{
  gboolean retval = FALSE;
  gchar *ui_path;
  GdictApplet *dictionary_applet = GDICT_APPLET (applet);
  GdictAppletPrivate *priv = dictionary_applet->priv;

  if (((!strcmp (iid, "DictionaryApplet"))) && gdict_create_data_dir ())
    {
      /* Set up the menu */
      priv->context_menu_action_group = gtk_action_group_new ("Dictionary Applet Actions");
      gtk_action_group_set_translation_domain(priv->context_menu_action_group,
                                              GETTEXT_PACKAGE);
      gtk_action_group_add_actions(priv->context_menu_action_group,
								gdict_applet_menu_actions,
								G_N_ELEMENTS (gdict_applet_menu_actions),
								applet);
      ui_path = g_build_filename(PKGDATADIR, "dictionary-applet-menu.xml", NULL);
      mate_panel_applet_setup_menu_from_file (applet, ui_path,
                                              priv->context_menu_action_group);
      g_free (ui_path);

      gtk_widget_show (GTK_WIDGET (applet));

      /* set the menu items insensitive */
      gdict_applet_set_menu_items_sensitive (dictionary_applet, FALSE);
      
      retval = TRUE;
    }

  return retval;
}

/* this defines the main () for the applet */
MATE_PANEL_APPLET_OUT_PROCESS_FACTORY ("DictionaryAppletFactory",
			     GDICT_TYPE_APPLET,
			     "mate-dictionary-applet",
			     gdict_applet_factory,
			     NULL);

