/* gdict-window.c - main application window
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <glib/gi18n.h>

#include <libgdict/gdict.h>

#include "gdict-sidebar.h"
#include "gdict-print.h"
#include "gdict-pref-dialog.h"
#include "gdict-about.h"
#include "gdict-window.h"
#include "gdict-common.h"

#define GDICT_WINDOW_COLUMNS      56
#define GDICT_WINDOW_ROWS         33

#define GDICT_WINDOW_MIN_WIDTH	  400
#define GDICT_WINDOW_MIN_HEIGHT	  330

/* sidebar pages logical ids */
#define GDICT_SIDEBAR_SPELLER_PAGE      "speller"
#define GDICT_SIDEBAR_DATABASES_PAGE    "db-chooser"
#define GDICT_SIDEBAR_STRATEGIES_PAGE   "strat-chooser"
#define GDICT_SIDEBAR_SOURCES_PAGE      "source-chooser"

enum
{
  COMPLETION_TEXT_COLUMN,

  COMPLETION_N_COLUMNS
};

enum
{
  PROP_0,

  PROP_ACTION,
  PROP_SOURCE_LOADER,
  PROP_SOURCE_NAME,
  PROP_DATABASE,
  PROP_STRATEGY,
  PROP_PRINT_FONT,
  PROP_DEFBOX_FONT,
  PROP_WORD,
  PROP_WINDOW_ID,

  LAST_PROP
};

enum
{
  CREATED,
  
  LAST_SIGNAL
};

static GParamSpec *gdict_window_properties[LAST_PROP] = { NULL, };
static guint gdict_window_signals[LAST_SIGNAL] = { 0 };

static const GtkTargetEntry drop_types[] =
{
  { "text/plain",    0, 0 },
  { "TEXT",          0, 0 },
  { "STRING",        0, 0 },
  { "UTF8_STRING",   0, 0 },
};
static const guint n_drop_types = G_N_ELEMENTS (drop_types);



G_DEFINE_TYPE (GdictWindow, gdict_window, GTK_TYPE_WINDOW);


static void
gdict_window_finalize (GObject *gobject)
{
  GdictWindow *window = GDICT_WINDOW (gobject);

  g_free (window->source_name);
  g_free (window->print_font);
  g_free (window->defbox_font);
  g_free (window->word);
  g_free (window->database);
  g_free (window->strategy);
  
  G_OBJECT_CLASS (gdict_window_parent_class)->finalize (gobject);
}

static void
gdict_window_dispose (GObject *gobject)
{
  GdictWindow *window = GDICT_WINDOW (gobject);

  if (window->desktop_settings != NULL)
    {
      g_object_unref (window->desktop_settings);
      window->desktop_settings = NULL;
    }

  if (window->settings != NULL)
    {
      g_object_unref (window->settings);

      window->settings = NULL;
    }
  
  if (window->context)
    {
      if (window->lookup_start_id)
        {
          g_signal_handler_disconnect (window->context, window->lookup_start_id);
          g_signal_handler_disconnect (window->context, window->definition_id);
          g_signal_handler_disconnect (window->context, window->lookup_end_id);
          g_signal_handler_disconnect (window->context, window->error_id);

          window->lookup_start_id = 0;
          window->definition_id = 0;
          window->lookup_end_id = 0;
          window->error_id = 0;
        }

      g_object_unref (window->context);
      window->context = NULL;
    }
  
  if (window->loader)
    {
      g_object_unref (window->loader);
      window->loader = NULL;
    }
  
  if (window->ui_manager)
    {
      g_object_unref (window->ui_manager);
      window->ui_manager = NULL;
    }
  
  if (window->action_group)
    {
      g_object_unref (window->action_group);
      window->action_group = NULL;
    }

  if (window->completion)
    {
      g_object_unref (window->completion);
      window->completion = NULL;
    }

  if (window->completion_model)
    {
      g_object_unref (window->completion_model);
      window->completion_model = NULL;
    }

  if (window->busy_cursor)
    {
      g_object_unref (window->busy_cursor);
      window->busy_cursor = NULL;
    }

  G_OBJECT_CLASS (gdict_window_parent_class)->dispose (gobject);
}

static const gchar *toggle_state[] = {
  "/MainMenu/FileMenu/SaveAsMenu",
  "/MainMenu/FileMenu/FilePreviewMenu",
  "/MainMenu/FileMenu/FilePrintMenu",
  "/MainMenu/GoMenu",
};

static gint n_toggle_state = G_N_ELEMENTS (toggle_state);

static void
gdict_window_ensure_menu_state (GdictWindow *window)
{
  gint i;
  gboolean is_sensitive;

  g_assert (GDICT_IS_WINDOW (window));

  if (!window->ui_manager)
    return;

  is_sensitive = !!(window->word != NULL);
  for (i = 0; i < n_toggle_state; i++)
    {
      GtkWidget *item;

      item = gtk_ui_manager_get_widget (window->ui_manager, toggle_state[i]);
      if (!item)
        continue;

      gtk_widget_set_sensitive (item, is_sensitive);
    }
}

static void
gdict_window_set_sidebar_visible (GdictWindow *window,
				  gboolean     is_visible)
{
  g_assert (GDICT_IS_WINDOW (window));

  is_visible = !!is_visible;

  if (is_visible != window->sidebar_visible)
    {
      GtkAction *action;

      window->sidebar_visible = is_visible;

      if (window->sidebar_visible)
	gtk_widget_show (window->sidebar_frame);
      else
	gtk_widget_hide (window->sidebar_frame);

      action = gtk_action_group_get_action (window->action_group, "ViewSidebar");
      gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), window->sidebar_visible);
    }
}

static void
gdict_window_set_statusbar_visible (GdictWindow *window,
				    gboolean     is_visible)
{
  g_assert (GDICT_IS_WINDOW (window));

  is_visible = !!is_visible;

  if (is_visible != window->statusbar_visible)
    {
      GtkAction *action;

      window->statusbar_visible = is_visible;

      if (window->statusbar_visible)
	gtk_widget_show (window->status);
      else
	gtk_widget_hide (window->status);

      action = gtk_action_group_get_action (window->action_group, "ViewStatusbar");
      gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), window->statusbar_visible);
    }
}

static void
gdict_window_definition_cb (GdictContext    *context,
			    GdictDefinition *definition,
			    GdictWindow     *window)
{
  gint total, n;
  gdouble fraction;

  g_assert (GDICT_IS_WINDOW (window));

  total = gdict_definition_get_total (definition);
  n = window->current_definition + 1;

  fraction = CLAMP (((gdouble) n / (gdouble) total), 0.0, 1.0);

  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (window->progress),
		  		 fraction);
  while (gtk_events_pending ())
    gtk_main_iteration ();

  window->current_definition = n;
}

static void
gdict_window_lookup_start_cb (GdictContext *context,
			      GdictWindow  *window)
{
  GdkDisplay *display;
  gchar *message;

  display = gtk_widget_get_display (GTK_WIDGET (window));

  if (!window->word)
    return;

  if (!window->busy_cursor)
    window->busy_cursor = gdk_cursor_new_for_display (display, GDK_WATCH);

  message = g_strdup_printf (_("Searching for '%s'..."), window->word);
  
  if (window->status && window->statusbar_visible)
    gtk_statusbar_push (GTK_STATUSBAR (window->status), 0, message);

  if (window->progress)
    gtk_widget_show (window->progress);

  window->max_definition = -1;
  window->last_definition = 0;
  window->current_definition = 0;

  gdk_window_set_cursor (gtk_widget_get_window (GTK_WIDGET (window)), window->busy_cursor);

  g_free (message);
}

static void
gdict_window_lookup_end_cb (GdictContext *context,
			    GdictWindow  *window)
{
  gchar *message;
  gint count;
  GtkTreeIter iter;
  GdictSource *source;
  GdictContext *speller_context;
  
  count = window->current_definition;

  window->max_definition = count - 1;

  if (count == 0)
    message = g_strdup (_("No definitions found"));
  else 
    message = g_strdup_printf (ngettext("A definition found",
					"%d definitions found",
					count),
		    	       count);

  if (window->status && window->statusbar_visible)
    gtk_statusbar_push (GTK_STATUSBAR (window->status), 0, message);

  if (window->progress)
    gtk_widget_hide (window->progress);

  /* we clone the context, so that the signals that it
   * fires do not get caught by the signal handlers we
   * use for getting the definitions.
   */
  source = gdict_source_loader_get_source (window->loader, window->source_name);
  speller_context = gdict_source_get_context (source);
  gdict_speller_set_context (GDICT_SPELLER (window->speller), speller_context);
  g_object_unref (speller_context);
  g_object_unref (source);

  /* search for similar words; if we have a no-match we already started
   * looking in the error signal handler
   */
  if (count != 0 && window->word)
    {
      gdict_speller_set_strategy (GDICT_SPELLER (window->speller), window->strategy);
      gdict_speller_match (GDICT_SPELLER (window->speller), window->word);
      gtk_list_store_append (window->completion_model, &iter);
      gtk_list_store_set (window->completion_model, &iter,
                          COMPLETION_TEXT_COLUMN, window->word,
                          -1);
    }

  gdk_window_set_cursor (gtk_widget_get_window (GTK_WIDGET (window)), NULL);
  g_free (message);

  if (count == 0)
    {
      g_free (window->word);
      window->word = NULL;
    }

  gdict_window_ensure_menu_state (window);
}

static void
gdict_window_error_cb (GdictContext *context,
		       const GError *error,
		       GdictWindow  *window)
{
  gdk_window_set_cursor (gtk_widget_get_window (GTK_WIDGET (window)), NULL);
  
  if (window->status && window->statusbar_visible)
    gtk_statusbar_push (GTK_STATUSBAR (window->status), 0,
                        _("No definitions found"));

  gtk_widget_hide (window->progress);

  /* launch the speller only on NO_MATCH */
  if (error->code == GDICT_CONTEXT_ERROR_NO_MATCH)
    {
      GdictSource *source;
      GdictContext *context;

      gdict_window_set_sidebar_visible (window, TRUE);
      gdict_sidebar_view_page (GDICT_SIDEBAR (window->sidebar),
                               GDICT_SIDEBAR_SPELLER_PAGE);

      /* we clone the context, so that the signals that it
       * fires do not get caught by the signal handlers we
       * use for getting the definitions.
       */
      source = gdict_source_loader_get_source (window->loader,
		      			       window->source_name);
      context = gdict_source_get_context (source);

      gdict_speller_set_context (GDICT_SPELLER (window->speller),
		      		 context);
      g_object_unref (context);
      g_object_unref (source);
      
      gdict_speller_set_strategy (GDICT_SPELLER (window->speller),
			          window->strategy);
      
      gdict_speller_match (GDICT_SPELLER (window->speller),
		           window->word);
    }

  /* unset the word and update the UI */
  g_free (window->word);
  window->word = NULL;

  gdict_window_ensure_menu_state (window);
}

static void
gdict_window_set_database (GdictWindow *window,
			   const gchar *database)
{
  if (g_strcmp0 (window->database, database) == 0)
    return;

  g_free (window->database);

  if (database != NULL && *database != '\0')
    window->database = g_strdup (database);
  else
    window->database = g_settings_get_string (window->settings, GDICT_SETTINGS_DATABASE_KEY);

  if (window->defbox)
    gdict_defbox_set_database (GDICT_DEFBOX (window->defbox),
			       window->database);

  if (window->db_chooser)
    gdict_database_chooser_set_current_database (GDICT_DATABASE_CHOOSER (window->db_chooser),
                                                 window->database);

  g_object_notify_by_pspec (G_OBJECT (window), gdict_window_properties[PROP_DATABASE]);
}

static void
gdict_window_set_strategy (GdictWindow *window,
			   const gchar *strategy)
{
  if (g_strcmp0 (window->strategy, strategy) == 0)
    return;

  g_free (window->strategy);

  if (strategy != NULL && *strategy != '\0')
    window->strategy = g_strdup (strategy);
  else
    window->strategy = g_settings_get_string (window->settings, GDICT_SETTINGS_STRATEGY_KEY);

  if (window->speller)
    gdict_speller_set_strategy (GDICT_SPELLER (window->speller),
                                window->strategy);

  if (window->strat_chooser)
    gdict_strategy_chooser_set_current_strategy (GDICT_STRATEGY_CHOOSER (window->strat_chooser),
                                                 window->strategy);

  g_object_notify_by_pspec (G_OBJECT (window), gdict_window_properties[PROP_STRATEGY]);
}

static GdictContext *
get_context_from_loader (GdictWindow *window)
{
  GdictSource *source;
  GdictContext *retval;

  if (!window->source_name)
    window->source_name = g_strdup (GDICT_DEFAULT_SOURCE_NAME);

  source = gdict_source_loader_get_source (window->loader,
		  			   window->source_name);
  if (!source &&
      strcmp (window->source_name, GDICT_DEFAULT_SOURCE_NAME) != 0)
    {
      g_free (window->source_name);
      window->source_name = g_strdup (GDICT_DEFAULT_SOURCE_NAME);

      source = gdict_source_loader_get_source (window->loader,
                                               window->source_name);
    }
  
  if (!source)
    {
      gchar *detail;
      
      detail = g_strdup_printf (_("No dictionary source available with name '%s'"),
      				window->source_name);

      gdict_show_error_dialog (GTK_WINDOW (window),
                               _("Unable to find dictionary source"),
                               detail);
      
      g_free (detail);

      return NULL;
    }
  
  gdict_window_set_database (window, gdict_source_get_database (source));
  gdict_window_set_strategy (window, gdict_source_get_strategy (source));
  
  retval = gdict_source_get_context (source);
  if (!retval)
    {
      gchar *detail;
      
      detail = g_strdup_printf (_("No context available for source '%s'"),
      				gdict_source_get_description (source));
      				
      gdict_show_error_dialog (GTK_WINDOW (window),
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
gdict_window_set_defbox_font (GdictWindow *window,
			      const gchar *defbox_font)
{
  g_free (window->defbox_font);

  if (defbox_font != NULL && *defbox_font != '\0')
    window->defbox_font = g_strdup (defbox_font);
  else
    window->defbox_font = g_settings_get_string (window->desktop_settings, DOCUMENT_FONT_KEY);

  gdict_defbox_set_font_name (GDICT_DEFBOX (window->defbox), window->defbox_font);
}

static void
gdict_window_set_print_font (GdictWindow *window,
			     const gchar *print_font)
{
  g_free (window->print_font);

  if (print_font != NULL && *print_font != '\0')
    window->print_font = g_strdup (print_font);
  else
    window->print_font = g_settings_get_string (window->settings, GDICT_SETTINGS_PRINT_FONT_KEY);
}

static void
gdict_window_set_word (GdictWindow *window,
		       const gchar *word,
		       const gchar *database)
{
  gchar *title;
  
  g_free (window->word);
  window->word = NULL;

  if (word && word[0] != '\0')
    window->word = g_strdup (word);
  else
    return;

  if (!database || database[0] == '\0')
    database = window->database;

  if (window->word)
    title = g_strdup_printf (_("%s - Dictionary"), window->word);
  else
    title = g_strdup (_("Dictionary"));
  
  gtk_window_set_title (GTK_WINDOW (window), title);
  g_free (title);

  if (window->defbox)
    {
      gdict_defbox_set_database (GDICT_DEFBOX (window->defbox), database);
      gdict_defbox_lookup (GDICT_DEFBOX (window->defbox), word);
    }
}

static void
gdict_window_set_context (GdictWindow  *window,
			  GdictContext *context)
{
  if (window->context)
    {
      g_signal_handler_disconnect (window->context, window->definition_id);
      g_signal_handler_disconnect (window->context, window->lookup_start_id);
      g_signal_handler_disconnect (window->context, window->lookup_end_id);
      g_signal_handler_disconnect (window->context, window->error_id);
      
      window->definition_id = 0;
      window->lookup_start_id = 0;
      window->lookup_end_id = 0;
      window->error_id = 0;
      
      g_object_unref (window->context);
      window->context = NULL;
    }

  if (window->defbox)
    gdict_defbox_set_context (GDICT_DEFBOX (window->defbox), context);

  if (window->db_chooser)
    gdict_database_chooser_set_context (GDICT_DATABASE_CHOOSER (window->db_chooser), context);

  if (window->strat_chooser)
    gdict_strategy_chooser_set_context (GDICT_STRATEGY_CHOOSER (window->strat_chooser), context);

  if (!context)
    return;
  
  /* attach our callbacks */
  window->definition_id   = g_signal_connect (context, "definition-found",
		  			      G_CALLBACK (gdict_window_definition_cb),
					      window);
  window->lookup_start_id = g_signal_connect (context, "lookup-start",
		  			      G_CALLBACK (gdict_window_lookup_start_cb),
					      window);
  window->lookup_end_id   = g_signal_connect (context, "lookup-end",
		  			      G_CALLBACK (gdict_window_lookup_end_cb),
					      window);
  window->error_id        = g_signal_connect (context, "error",
		  			      G_CALLBACK (gdict_window_error_cb),
					      window);
  
  window->context = context;
}

static void
gdict_window_set_source_name (GdictWindow *window,
			      const gchar *source_name)
{
  GdictContext *context;

  if (window->source_name && source_name &&
      strcmp (window->source_name, source_name) == 0)
    return;

  g_free (window->source_name);

  if (source_name != NULL && *source_name != '\0')
    window->source_name = g_strdup (source_name);
  else
    window->source_name = g_settings_get_string (window->settings, GDICT_SETTINGS_SOURCE_KEY);

  context = get_context_from_loader (window);
  gdict_window_set_context (window, context);

  if (window->source_chooser)
    gdict_source_chooser_set_current_source (GDICT_SOURCE_CHOOSER (window->source_chooser),
                                             window->source_name);

  g_object_notify_by_pspec (G_OBJECT (window), gdict_window_properties[PROP_SOURCE_NAME]);
}

static void
gdict_window_set_property (GObject      *object,
			   guint         prop_id,
			   const GValue *value,
			   GParamSpec   *pspec)
{
  GdictWindow *window = GDICT_WINDOW (object);
  
  switch (prop_id)
    {
    case PROP_ACTION:
      window->action = g_value_get_enum (value);
      break;
    case PROP_SOURCE_LOADER:
      if (window->loader)
        g_object_unref (window->loader);
      window->loader = g_value_get_object (value);
      g_object_ref (window->loader);
      break;
    case PROP_SOURCE_NAME:
      gdict_window_set_source_name (window, g_value_get_string (value));
      break;
    case PROP_DATABASE:
      gdict_window_set_database (window, g_value_get_string (value));
      break;
    case PROP_STRATEGY:
      gdict_window_set_strategy (window, g_value_get_string (value));
      break;
    case PROP_WORD:
      gdict_window_set_word (window, g_value_get_string (value), NULL);
      break;
    case PROP_PRINT_FONT:
      gdict_window_set_print_font (window, g_value_get_string (value));
      break;
    case PROP_DEFBOX_FONT:
      gdict_window_set_defbox_font (window, g_value_get_string (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gdict_window_get_property (GObject    *object,
			   guint       prop_id,
			   GValue     *value,
			   GParamSpec *pspec)
{
  GdictWindow *window = GDICT_WINDOW (object);
  
  switch (prop_id)
    {
    case PROP_ACTION:
      g_value_set_enum (value, window->action);
      break;
    case PROP_SOURCE_LOADER:
      g_value_set_object (value, window->loader);
      break;
    case PROP_SOURCE_NAME:
      g_value_set_string (value, window->source_name);
      break;
    case PROP_DATABASE:
      g_value_set_string (value, window->database);
      break;
    case PROP_STRATEGY:
      g_value_set_string (value, window->strategy);
      break;
    case PROP_WORD:
      g_value_set_string (value, window->word);
      break;
    case PROP_PRINT_FONT:
      g_value_set_string (value, window->print_font);
      break;
    case PROP_DEFBOX_FONT:
      g_value_set_string (value, window->defbox_font);
      break;
    case PROP_WINDOW_ID:
      g_value_set_uint (value, window->window_id);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gdict_window_store_state (GdictWindow *window)
{
  gchar *state_dir, *state_file;
  GKeyFile *state_key;
  gchar *data;
  gsize data_len;
  GError *error;
  const gchar *page_id;

  state_dir = g_build_filename (g_get_user_config_dir (),
                                "mate",
                                "mate-dictionary",
                                NULL);

  if (g_mkdir (state_dir, 0700) == -1)
    {
      if (errno != EEXIST)
        {
          g_warning ("Unable to create a cache directory: %s", g_strerror (errno));
          g_free (state_dir);
          return;
        }
    }

  state_file = g_build_filename (state_dir, "window.ini", NULL);
  state_key = g_key_file_new ();

  /* store the default size of the window and its state, so that
   * it's picked up by newly created windows
   */
  g_key_file_set_integer (state_key, "WindowState", "Width", window->current_width);
  g_key_file_set_integer (state_key, "WindowState", "Height", window->current_height);
  g_key_file_set_boolean (state_key, "WindowState", "IsMaximized", window->is_maximized);
  g_key_file_set_boolean (state_key, "WindowState", "SidebarVisible", window->sidebar_visible);
  g_key_file_set_boolean (state_key, "WindowState", "StatusbarVisible", window->statusbar_visible);
  g_key_file_set_integer (state_key, "WindowState", "SidebarWidth", window->sidebar_width);

  page_id = gdict_sidebar_current_page (GDICT_SIDEBAR (window->sidebar));
  if (page_id == NULL)
    page_id = GDICT_SIDEBAR_SPELLER_PAGE;

  g_key_file_set_string (state_key, "WindowState", "SidebarPage", page_id);

  error = NULL;
  data = g_key_file_to_data (state_key, &data_len, &error);
  if (error != NULL)
    {
      g_warning ("Unable to create the window state file: %s", error->message);
      g_error_free (error);
    }
  else
    {
      g_file_set_contents (state_file, data, data_len, &error);
      if (error != NULL)
        {
          g_warning ("Unable to write the window state file: %s", error->message);
          g_error_free (error);
        }

      g_free (data);
    }

  g_key_file_free (state_key);
  g_free (state_file);
  g_free (state_dir);
}

static void
gdict_window_load_state (GdictWindow *window)
{
  gchar *state_file;
  GKeyFile *state_key;
  gchar *data;
  gsize data_len;
  GError *error;

  state_file = g_build_filename (g_get_user_config_dir (),
                                 "mate",
                                 "mate-dictionary",
                                 "window.ini",
                                 NULL);
  state_key = g_key_file_new ();

  error = NULL;
  g_key_file_load_from_file (state_key, state_file, 0, &error);
  if (error != NULL)
    {
      g_warning ("Unable to load the window state file: %s", error->message);
      g_error_free (error);
      g_key_file_free (state_key);
      g_free (state_file);
      return;
    }

  window->default_width = g_key_file_get_integer (state_key, "WindowState", "Width", &error);
  if (error != NULL)
    {
      g_clear_error (&error);
      window->default_width = -1;
    }

  window->default_height = g_key_file_get_integer (state_key, "WindowState", "Height", &error);
  if (error != NULL)
    {
      g_clear_error (&error);
      window->default_height = -1;
    }

  window->is_maximized = g_key_file_get_boolean (state_key, "WindowState", "IsMaximized", &error);
  if (error != NULL)
    {
      g_clear_error (&error);
      window->is_maximized = FALSE;
    }

  window->sidebar_visible = g_key_file_get_boolean (state_key, "WindowState", "SidebarVisible", &error);
  if (error != NULL)
    {
      g_clear_error (&error);
      window->sidebar_visible = FALSE;
    }

  window->statusbar_visible = g_key_file_get_boolean (state_key, "WindowState", "StatusbarVisible", &error);
  if (error != NULL)
    {
      g_clear_error (&error);
      window->statusbar_visible = FALSE;
    }

  window->sidebar_width = g_key_file_get_integer (state_key, "WindowState", "SidebarWidth", &error);
  if (error != NULL)
    {
      g_clear_error (&error);
      window->sidebar_width = -1;
    }

  window->sidebar_page = g_key_file_get_string (state_key, "WindowState", "SidebarPage", &error);
  if (error != NULL)
    {
      g_clear_error (&error);
      window->sidebar_page = NULL;
    }

  g_key_file_free (state_key);
  g_free (state_file);
}

static void
gdict_window_cmd_file_new (GtkAction   *action,
			   GdictWindow *window)
{
  GtkWidget *new_window;
  gchar *word = NULL;

  gdict_window_store_state (window);

  word = gdict_defbox_get_selected_word (GDICT_DEFBOX (window->defbox));
  if (word)
    {
      new_window = gdict_window_new (GDICT_WINDOW_ACTION_LOOKUP,
                                     window->loader,
                                     NULL, word);
      g_free (word);
    }
  else
    new_window = gdict_window_new (GDICT_WINDOW_ACTION_CLEAR,
                                   window->loader,
                                   NULL, NULL);

  gtk_widget_show (new_window);
  
  g_signal_emit (window, gdict_window_signals[CREATED], 0, new_window);
}

static void
gdict_window_cmd_save_as (GtkAction   *action,
			  GdictWindow *window)
{
  GtkWidget *dialog;
  
  g_assert (GDICT_IS_WINDOW (window));
  
  dialog = gtk_file_chooser_dialog_new (_("Save a Copy"),
  					GTK_WINDOW (window),
  					GTK_FILE_CHOOSER_ACTION_SAVE,
  					GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
  					GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
  					NULL);
  gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);
  
  /* default to user's home */
  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), g_get_home_dir ());
  gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (dialog), _("Untitled document"));
  
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {
      gchar *filename;
      gchar *text;
      gsize len;
      GError *write_error = NULL;
      
      filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
      
      text = gdict_defbox_get_text (GDICT_DEFBOX (window->defbox), &len);
      
      g_file_set_contents (filename,
      			   text,
      			   len,
      			   &write_error);
      if (write_error)
        {
          gchar *message;
          
          message = g_strdup_printf (_("Error while writing to '%s'"), filename);
          
          gdict_show_gerror_dialog (GTK_WINDOW (window),
                                    message,
                                    write_error);

          g_free (message);
        }
      
      g_free (text);
      g_free (filename);
    }

  gtk_widget_destroy (dialog);
}

static void
gdict_window_cmd_file_preview (GtkAction   *action,
                               GdictWindow *window)
{
  g_assert (GDICT_IS_WINDOW (window));

  gdict_show_print_preview (GTK_WINDOW (window),
                            GDICT_DEFBOX (window->defbox));
}

static void
gdict_window_cmd_file_print (GtkAction   *action,
			     GdictWindow *window)
{
  g_assert (GDICT_IS_WINDOW (window));
  
  gdict_show_print_dialog (GTK_WINDOW (window),
  			   GDICT_DEFBOX (window->defbox));
}

static void
gdict_window_cmd_file_close_window (GtkAction   *action,
				    GdictWindow *window)
{
  g_assert (GDICT_IS_WINDOW (window));

  gdict_window_store_state (window);

  /* if this was called from the uimanager, destroy the widget;
   * otherwise, if it was called from the delete_event, the widget
   * will destroy itself.
   */
  if (action)
    gtk_widget_destroy (GTK_WIDGET (window));
}

static void
gdict_window_cmd_edit_copy (GtkAction   *action,
			    GdictWindow *window)
{
  g_assert (GDICT_IS_WINDOW (window));

  if (gtk_widget_has_focus (window->entry))
    gtk_editable_copy_clipboard (GTK_EDITABLE (window->entry));
  else
    gdict_defbox_copy_to_clipboard (GDICT_DEFBOX (window->defbox),
                                    gtk_clipboard_get (GDK_SELECTION_CLIPBOARD));
}

static void
gdict_window_cmd_edit_select_all (GtkAction   *action,
				  GdictWindow *window)
{
  g_assert (GDICT_IS_WINDOW (window));

  if (gtk_widget_has_focus (window->entry))
    gtk_editable_select_region (GTK_EDITABLE (window->entry), 0, -1);
  else
    gdict_defbox_select_all (GDICT_DEFBOX (window->defbox));
}

static void
gdict_window_cmd_edit_find (GtkAction   *action,
			    GdictWindow *window)
{
  g_assert (GDICT_IS_WINDOW (window));
  
  gdict_defbox_set_show_find (GDICT_DEFBOX (window->defbox), TRUE);
}

static void
gdict_window_cmd_edit_find_next (GtkAction   *action,
				 GdictWindow *window)
{
  g_assert (GDICT_IS_WINDOW (window));

  gdict_defbox_find_next (GDICT_DEFBOX (window->defbox));
}

static void
gdict_window_cmd_edit_find_previous (GtkAction   *action,
				     GdictWindow *window)
{
  g_assert (GDICT_IS_WINDOW (window));

  gdict_defbox_find_previous (GDICT_DEFBOX (window->defbox));
}

static void
gdict_window_cmd_edit_preferences (GtkAction   *action,
				   GdictWindow *window)
{
  g_assert (GDICT_IS_WINDOW (window));
  
  gdict_show_pref_dialog (GTK_WIDGET (window),
  			  _("Dictionary Preferences"),
  			  window->loader);
}

static void
gdict_window_cmd_view_sidebar (GtkToggleAction *action,
			       GdictWindow     *window)
{
  g_assert (GDICT_IS_WINDOW (window));

  window->sidebar_visible = gtk_toggle_action_get_active (action);

  if (window->sidebar_visible)
    gtk_widget_show (window->sidebar_frame);
  else
    gtk_widget_hide (window->sidebar_frame);
}

static void
gdict_window_cmd_view_statusbar (GtkToggleAction *action,
				 GdictWindow     *window)
{
  g_assert (GDICT_IS_WINDOW (window));

  window->statusbar_visible = gtk_toggle_action_get_active (action);

  if (window->statusbar_visible)
    gtk_widget_show (window->status);
  else
    gtk_widget_hide (window->status);

}

static void
gdict_window_cmd_view_speller (GtkAction   *action,
			       GdictWindow *window)
{
  g_assert (GDICT_IS_WINDOW (window));

  gdict_sidebar_view_page (GDICT_SIDEBAR (window->sidebar),
                           GDICT_SIDEBAR_SPELLER_PAGE);
  gdict_window_set_sidebar_visible (window, TRUE);
}

static void
gdict_window_cmd_view_databases (GtkAction   *action,
				 GdictWindow *window)
{
  g_assert (GDICT_IS_WINDOW (window));

  gdict_sidebar_view_page (GDICT_SIDEBAR (window->sidebar),
                           GDICT_SIDEBAR_DATABASES_PAGE);
  gdict_window_set_sidebar_visible (window, TRUE);
}

static void
gdict_window_cmd_view_strategies (GtkAction   *action,
                                  GdictWindow *window)
{
  g_assert (GDICT_IS_WINDOW (window));

  gdict_sidebar_view_page (GDICT_SIDEBAR (window->sidebar),
                           GDICT_SIDEBAR_STRATEGIES_PAGE);
  gdict_window_set_sidebar_visible (window, TRUE);
}

static void
gdict_window_cmd_view_sources (GtkAction   *action,
                               GdictWindow *window)
{
  g_assert (GDICT_IS_WINDOW (window));

  gdict_sidebar_view_page (GDICT_SIDEBAR (window->sidebar),
                           GDICT_SIDEBAR_SOURCES_PAGE);
  gdict_window_set_sidebar_visible (window, TRUE);
}

static void
gdict_window_cmd_go_first_def (GtkAction   *action,
			       GdictWindow *window)
{
  g_assert (GDICT_IS_WINDOW (window));
  
  window->last_definition = 0;
  gdict_defbox_jump_to_definition (GDICT_DEFBOX (window->defbox),
                                   window->last_definition);
}

static void
gdict_window_cmd_go_previous_def (GtkAction   *action,
				  GdictWindow *window)
{
  g_assert (GDICT_IS_WINDOW (window));
  
  if (window->last_definition == 0)
    return;
  
  window->last_definition -= 1;
  gdict_defbox_jump_to_definition (GDICT_DEFBOX (window->defbox),
                                   window->last_definition);
}

static void
gdict_window_cmd_go_next_def (GtkAction   *action,
			      GdictWindow *window)
{
  g_assert (GDICT_IS_WINDOW (window));
  
  if (window->max_definition == -1)
    window->max_definition = gdict_defbox_count_definitions (GDICT_DEFBOX (window->defbox)) - 1;
    
  if (window->last_definition == window->max_definition)
    return;
  
  window->last_definition += 1;
  gdict_defbox_jump_to_definition (GDICT_DEFBOX (window->defbox),
                                   window->last_definition);
}

static void
gdict_window_cmd_go_last_def (GtkAction   *action,
			      GdictWindow *window)
{
  g_assert (GDICT_IS_WINDOW (window));
  
  if (window->max_definition == -1)
    window->last_definition = gdict_defbox_count_definitions (GDICT_DEFBOX (window->defbox)) - 1;
  
  window->last_definition = window->max_definition;
  gdict_defbox_jump_to_definition (GDICT_DEFBOX (window->defbox),
                                   window->last_definition);
}

static void
gdict_window_cmd_help_contents (GtkAction   *action,
				GdictWindow *window)
{
  GError *err = NULL;
  
  g_return_if_fail (GDICT_IS_WINDOW (window));
 
  gtk_show_uri (gtk_widget_get_screen (GTK_WIDGET (window)),
                "help:mate-dictionary",
                gtk_get_current_event_time (), &err); 
  if (err)
    {
      gdict_show_gerror_dialog (GTK_WINDOW (window),
      		                _("There was an error while displaying help"),
      			        err);
    }
}

static void
gdict_window_cmd_help_about (GtkAction   *action,
			     GdictWindow *window)
{
  g_assert (GDICT_IS_WINDOW (window));
  
  gdict_show_about_dialog (GTK_WIDGET (window));
}

static void
gdict_window_cmd_lookup (GtkAction   *action,
			 GdictWindow *window)
{
  g_assert (GDICT_IS_WINDOW (window));

  gtk_widget_grab_focus (window->entry);
}

static void
gdict_window_cmd_escape (GtkAction   *action,
			 GdictWindow *window)
{
  g_assert (GDICT_IS_WINDOW (window));
  
  gdict_defbox_set_show_find (GDICT_DEFBOX (window->defbox), FALSE);
}

static const GtkActionEntry entries[] =
{
  { "File", NULL, N_("_File") },
  { "Edit", NULL, N_("_Edit") },
  { "View", NULL, N_("_View") },
  { "Go", NULL, N_("_Go") },
  { "Help", NULL, N_("_Help") },

  /* File menu */
  { "FileNew", GTK_STOCK_NEW, N_("_New"), "<control>N",
    N_("New look up"), G_CALLBACK (gdict_window_cmd_file_new) },
  { "FileSaveAs", GTK_STOCK_SAVE_AS, N_("_Save a Copy..."), NULL, NULL,
    G_CALLBACK (gdict_window_cmd_save_as) },
  { "FilePreview", NULL, N_("P_review..."), "<control><shift>P",
    N_("Preview this document"), G_CALLBACK (gdict_window_cmd_file_preview) },
  { "FilePrint", GTK_STOCK_PRINT, N_("_Print..."), "<control>P",
    N_("Print this document"), G_CALLBACK (gdict_window_cmd_file_print) },
  { "FileCloseWindow", GTK_STOCK_CLOSE, NULL, "<control>W", NULL,
    G_CALLBACK (gdict_window_cmd_file_close_window) },

  /* Edit menu */
  { "EditCopy", GTK_STOCK_COPY, NULL, "<control>C", NULL,
    G_CALLBACK (gdict_window_cmd_edit_copy) },
  { "EditSelectAll", NULL, N_("Select _All"), "<control>A", NULL,
    G_CALLBACK (gdict_window_cmd_edit_select_all) },
  { "EditFind", GTK_STOCK_FIND, NULL, "<control>F",
    N_("Find a word or phrase in the document"),
    G_CALLBACK (gdict_window_cmd_edit_find) },
  { "EditFindNext", NULL, N_("Find Ne_xt"), "<control>G", NULL,
    G_CALLBACK (gdict_window_cmd_edit_find_next) },
  { "EditFindPrevious", NULL, N_("Find Pre_vious"), "<control><shift>G", NULL,
    G_CALLBACK (gdict_window_cmd_edit_find_previous) },
  { "EditPreferences", GTK_STOCK_PREFERENCES, N_("_Preferences"), NULL, NULL,
    G_CALLBACK (gdict_window_cmd_edit_preferences) },

  /* Go menu */
  { "GoPreviousDef", GTK_STOCK_GO_BACK, N_("_Previous Definition"), "<control>Page_Up",
    N_("Go to the previous definition"), G_CALLBACK (gdict_window_cmd_go_previous_def) },
  { "GoNextDef", GTK_STOCK_GO_FORWARD, N_("_Next Definition"), "<control>Page_Down",
    N_("Go to the next definition"), G_CALLBACK (gdict_window_cmd_go_next_def) },
  { "GoFirstDef", GTK_STOCK_GOTO_FIRST, N_("_First Definition"), "<control>Home",
    N_("Go to the first definition"), G_CALLBACK (gdict_window_cmd_go_first_def) },
  { "GoLastDef", GTK_STOCK_GOTO_LAST, N_("_Last Definition"), "<control>End",
    N_("Go to the last definition"), G_CALLBACK (gdict_window_cmd_go_last_def) },

  /* View menu */
  { "ViewSpeller", NULL, N_("Similar _Words"), "<control>T", NULL,
    G_CALLBACK (gdict_window_cmd_view_speller), },
  { "ViewSource", NULL, N_("Dictionary Sources"), "<control>D", NULL,
    G_CALLBACK (gdict_window_cmd_view_sources), },
  { "ViewDB", NULL, N_("Available _Databases"), "<control>B", NULL,
    G_CALLBACK (gdict_window_cmd_view_databases), },
  { "ViewStrat", NULL, N_("Available St_rategies"), "<control>R", NULL,
    G_CALLBACK (gdict_window_cmd_view_strategies), },

  /* Help menu */
  { "HelpContents", GTK_STOCK_HELP, N_("_Contents"), "F1", NULL,
    G_CALLBACK (gdict_window_cmd_help_contents) },
  { "HelpAbout", GTK_STOCK_ABOUT, N_("_About"), NULL, NULL,
    G_CALLBACK (gdict_window_cmd_help_about) },
  
  /* Accelerators */
  { "Lookup", NULL, "", "<control>L", NULL, G_CALLBACK (gdict_window_cmd_lookup) },
  { "Escape", NULL, "", "Escape", "", G_CALLBACK (gdict_window_cmd_escape) },
  { "Slash", GTK_STOCK_FIND, NULL, "slash", NULL, G_CALLBACK (gdict_window_cmd_edit_find) },
};

static const GtkToggleActionEntry toggle_entries[] = {
  /* View menu */
  { "ViewSidebar", NULL, N_("_Sidebar"), "F9", NULL,
    G_CALLBACK (gdict_window_cmd_view_sidebar), FALSE },
  { "ViewStatusbar", NULL, N_("S_tatusbar"), NULL, NULL,
    G_CALLBACK (gdict_window_cmd_view_statusbar), FALSE },
};

static gboolean
gdict_window_delete_event_cb (GtkWidget *widget,
			      GdkEvent  *event,
			      gpointer   user_data)
{
  gdict_window_cmd_file_close_window (NULL, GDICT_WINDOW (widget));

  return FALSE;
}

static gboolean
gdict_window_state_event_cb (GtkWidget           *widget,
			     GdkEventWindowState *event,
			     gpointer             user_data)
{
  GdictWindow *window = GDICT_WINDOW (widget);
  
  if (event->new_window_state & GDK_WINDOW_STATE_MAXIMIZED)
    window->is_maximized = TRUE;
  else
    window->is_maximized = FALSE;
  
  return FALSE;
}

static void
lookup_word (GdictWindow *window,
             gpointer     dummy)
{
  const gchar *entry_text;
  gchar *word;
  
  g_assert (GDICT_IS_WINDOW (window));
  
  if (!window->context)
    return;
  
  entry_text = gtk_entry_get_text (GTK_ENTRY (window->entry));
  if (!entry_text || *entry_text == '\0')
    return;

  word = g_strdup (entry_text);
  gdict_window_set_word (window, g_strstrip (word), NULL);

  g_free (word);
}

static void
source_activated_cb (GdictSourceChooser *chooser,
                     const gchar        *source_name,
                     GdictSource        *source,
                     GdictWindow        *window)
{
  g_signal_handlers_block_by_func (chooser, source_activated_cb, window);
  gdict_window_set_source_name (window, source_name);
  g_signal_handlers_unblock_by_func (chooser, source_activated_cb, window);

  if (window->status && window->statusbar_visible)
    {
      gchar *message;

      message = g_strdup_printf (_("Dictionary source `%s' selected"),
                                 gdict_source_get_description (source));
      gtk_statusbar_push (GTK_STATUSBAR (window->status), 0, message);
      g_free (message);
    }
}

static void
strategy_activated_cb (GdictStrategyChooser *chooser,
                       const gchar          *strat_name,
                       const gchar          *strat_desc,
                       GdictWindow          *window)
{
  g_signal_handlers_block_by_func (chooser, strategy_activated_cb, window);
  gdict_window_set_strategy (window, strat_name);
  g_signal_handlers_unblock_by_func (chooser, strategy_activated_cb, window);

  if (window->status && window->statusbar_visible)
    {
      gchar *message;

      message = g_strdup_printf (_("Strategy `%s' selected"), strat_desc);
      gtk_statusbar_push (GTK_STATUSBAR (window->status), 0, message);
      g_free (message);
    }
}

static void
database_activated_cb (GdictDatabaseChooser *chooser,
		       const gchar          *db_name,
		       const gchar          *db_desc,
		       GdictWindow          *window)
{
  g_signal_handlers_block_by_func (chooser, database_activated_cb, window);
  gdict_window_set_database (window, db_name);
  g_signal_handlers_unblock_by_func (chooser, database_activated_cb, window);

  if (window->status && window->statusbar_visible)
    {
      gchar *message;

      message = g_strdup_printf (_("Database `%s' selected"), db_desc);
      gtk_statusbar_push (GTK_STATUSBAR (window->status), 0, message);
      g_free (message);
    }
}

static void
speller_word_activated_cb (GdictSpeller *speller,
			   const gchar  *word,
			   const gchar  *db_name,
			   GdictWindow  *window)
{
  gtk_entry_set_text (GTK_ENTRY (window->entry), word);
  
  gdict_window_set_word (window, word, db_name);

  if (window->status && window->statusbar_visible)
    {
      gchar *message;

      message = g_strdup_printf (_("Word `%s' selected"), word);
      gtk_statusbar_push (GTK_STATUSBAR (window->status), 0, message);
      g_free (message);
    }
}

static void
sidebar_page_changed_cb (GdictSidebar *sidebar,
			 GdictWindow  *window)
{
  const gchar *page_id;
  const gchar *message;

  page_id = gdict_sidebar_current_page (sidebar);

  g_free (window->sidebar_page);
  window->sidebar_page = g_strdup (page_id);

  switch (page_id[0])
    {
    case 's':
      {
      switch (page_id[1])
        {
        case 'p': /* speller */
          message = _("Double-click on the word to look up");
          if (window->word)
            gdict_speller_match (GDICT_SPELLER (window->speller),
                                 window->word);
          break;
        case 't': /* strat-chooser */
          message = _("Double-click on the matching strategy to use");
          
          gdict_strategy_chooser_refresh (GDICT_STRATEGY_CHOOSER (window->strat_chooser));
          break;
        case 'o': /* source-chooser */
          message = _("Double-click on the source to use");
          gdict_source_chooser_refresh (GDICT_SOURCE_CHOOSER (window->source_chooser));
          break;
        default:
          message = NULL;
        }
      }
      break;
    case 'd': /* db-chooser */
      message = _("Double-click on the database to use");
      
      gdict_database_chooser_refresh (GDICT_DATABASE_CHOOSER (window->db_chooser));
      break;
    default:
      message = NULL;
      break;
    }

  if (message && window->status && window->statusbar_visible)
    gtk_statusbar_push (GTK_STATUSBAR (window->status), 0, message);
}

static void
sidebar_closed_cb (GdictSidebar *sidebar,
		   GdictWindow  *window)
{
  gdict_window_set_sidebar_visible (window, FALSE); 
}

static void
gdict_window_link_clicked (GdictDefbox *defbox,
                           const gchar *link_text,
                           GdictWindow *window)
{
  GtkWidget *new_window;

  gdict_window_store_state (window);

  new_window = gdict_window_new (GDICT_WINDOW_ACTION_LOOKUP,
                                 window->loader,
                                 NULL, link_text);
  gtk_widget_show (new_window);
  
  g_signal_emit (window, gdict_window_signals[CREATED], 0, new_window);
}

static void
gdict_window_drag_data_received_cb (GtkWidget        *widget,
				    GdkDragContext   *context,
				    gint              x,
				    gint              y,
				    GtkSelectionData *data,
				    guint             info,
				    guint             time_,
				    gpointer          user_data)
{
  GdictWindow *window = GDICT_WINDOW (user_data);
  gchar *text;
  
  text = (gchar *) gtk_selection_data_get_text (data);
  if (text)
    {
      gtk_entry_set_text (GTK_ENTRY (window->entry), text);

      gdict_window_set_word (window, text, NULL);
      g_free (text);
      
      gtk_drag_finish (context, TRUE, FALSE, time_);
    }
  else
    gtk_drag_finish (context, FALSE, FALSE, time_);
}

static void
gdict_window_size_allocate (GtkWidget     *widget,
			    GtkAllocation *allocation)
{
  GdictWindow *window = GDICT_WINDOW (widget);

  if (!window->is_maximized)
    {
      window->current_width = allocation->width;
      window->current_height = allocation->height;
    }

  if (GTK_WIDGET_CLASS (gdict_window_parent_class)->size_allocate)
    GTK_WIDGET_CLASS (gdict_window_parent_class)->size_allocate (widget,
		    						 allocation);
}

static void
gdict_window_handle_notify_position_cb (GtkWidget  *widget,
					GParamSpec *pspec,
					gpointer    user_data)
{
  GdictWindow *window = GDICT_WINDOW (user_data);
  gint window_width, pos;
  GtkAllocation allocation;

  pos = gtk_paned_get_position (GTK_PANED (widget));
  gtk_widget_get_allocation (GTK_WIDGET (window), &allocation);
  window_width = allocation.width;

  window->sidebar_width = window_width - pos;
}

static GObject *
gdict_window_constructor (GType                  type,
			  guint                  n_construct_properties,
			  GObjectConstructParam *construct_params)
{
  GObject *object;
  GdictWindow *window;
  gboolean is_maximized;
  GtkWidget *hbox;
  GtkWidget *handle;
  GtkWidget *frame1, *frame2;
  GtkWidget *vbox;
  GtkWidget *button;
  GtkActionGroup *action_group;
  GtkAccelGroup *accel_group;
  PangoFontDescription *font_desc;
  gchar *font_name, *sidebar_page;
  GError *error;
  gboolean sidebar_visible;
  gboolean statusbar_visible;
  GtkAllocation allocation;
  
  object = G_OBJECT_CLASS (gdict_window_parent_class)->constructor (type, n_construct_properties, construct_params);
  window = GDICT_WINDOW (object);

  window->in_construction = TRUE;

  /* recover the state */
  gdict_window_load_state (window);

  window->main_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add (GTK_CONTAINER (window), window->main_box);
  gtk_widget_show (window->main_box);
  
  /* build menus */
  action_group = gtk_action_group_new ("MenuActions");
  window->action_group = action_group;
  gtk_action_group_set_translation_domain (action_group, NULL);
  gtk_action_group_add_actions (action_group, entries,
  				G_N_ELEMENTS (entries),
  				window);
  gtk_action_group_add_toggle_actions (action_group, toggle_entries,
                                       G_N_ELEMENTS (toggle_entries),
                                       window);
  
  window->ui_manager = gtk_ui_manager_new ();
  gtk_ui_manager_insert_action_group (window->ui_manager, action_group, 0);
  
  accel_group = gtk_ui_manager_get_accel_group (window->ui_manager);
  gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);
  
  error = NULL;
  if (!gtk_ui_manager_add_ui_from_file (window->ui_manager,
  					PKGDATADIR "/mate-dictionary-ui.xml",
  					&error))
    {
      g_warning ("Building menus failed: %s", error->message);
      g_error_free (error);
    }
  else
    {
      window->menubar = gtk_ui_manager_get_widget (window->ui_manager, "/MainMenu");
      
      gtk_box_pack_start (GTK_BOX (window->main_box), window->menubar, FALSE, FALSE, 0);
      gtk_widget_show (window->menubar);

      gdict_window_ensure_menu_state (window);
    }
  
  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 6);
  gtk_container_add (GTK_CONTAINER (window->main_box), vbox);
  gtk_widget_show (vbox);
  
  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 12);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
  gtk_widget_show (hbox);
  
  button = gtk_button_new_with_mnemonic (_("Look _up:"));
  g_signal_connect_swapped (button, "clicked",
                            G_CALLBACK (lookup_word),
                            window);
  gtk_button_set_relief (GTK_BUTTON (button), GTK_RELIEF_NONE);
  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
  gtk_widget_show (button);

  window->completion_model = gtk_list_store_new (COMPLETION_N_COLUMNS,
		  				 G_TYPE_STRING);
  
  window->completion = gtk_entry_completion_new ();
  gtk_entry_completion_set_popup_completion (window->completion, TRUE);
  gtk_entry_completion_set_model (window->completion,
		  		  GTK_TREE_MODEL (window->completion_model));
  gtk_entry_completion_set_text_column (window->completion,
		  			COMPLETION_TEXT_COLUMN);
  
  window->entry = gtk_entry_new ();
  if (window->word)
    gtk_entry_set_text (GTK_ENTRY (window->entry), window->word);
  
  gtk_entry_set_completion (GTK_ENTRY (window->entry),
		  	    window->completion);
  g_signal_connect_swapped (window->entry, "activate",
                            G_CALLBACK (lookup_word),
                            window);
  gtk_box_pack_start (GTK_BOX (hbox), window->entry, TRUE, TRUE, 0);
  gtk_widget_show (window->entry);

  handle = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
  gtk_box_pack_start (GTK_BOX (vbox), handle, TRUE, TRUE, 0);
  gtk_widget_show (handle);

  frame1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  frame2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  
  window->defbox = gdict_defbox_new ();
  if (window->context)
    gdict_defbox_set_context (GDICT_DEFBOX (window->defbox), window->context);

  g_signal_connect (window->defbox, "link-clicked",
                    G_CALLBACK (gdict_window_link_clicked),
                    window);

  gtk_drag_dest_set (window->defbox,
  		     GTK_DEST_DEFAULT_ALL,
  		     drop_types, n_drop_types,
  		     GDK_ACTION_COPY);
  g_signal_connect (window->defbox, "drag-data-received",
  		    G_CALLBACK (gdict_window_drag_data_received_cb),
  		    window);
  gtk_container_add (GTK_CONTAINER (frame1), window->defbox);
  gtk_widget_show (window->defbox);

  /* Sidebar */
  window->sidebar = gdict_sidebar_new ();
  g_signal_connect (window->sidebar, "page-changed",
		    G_CALLBACK (sidebar_page_changed_cb),
		    window);
  g_signal_connect (window->sidebar, "closed",
		    G_CALLBACK (sidebar_closed_cb),
		    window);
  gtk_widget_show (window->sidebar);
  
  /* Speller */
  window->speller = gdict_speller_new ();
  if (window->context)
    gdict_speller_set_context (GDICT_SPELLER (window->speller),
		    	       window->context);
  g_signal_connect (window->speller, "word-activated",
		    G_CALLBACK (speller_word_activated_cb),
		    window);
  gdict_sidebar_add_page (GDICT_SIDEBAR (window->sidebar),
		          GDICT_SIDEBAR_SPELLER_PAGE,
			  _("Similar words"),
			  window->speller);
  gtk_widget_show (window->speller);

  /* Database chooser */
  if (window->context)
    gdict_database_chooser_set_context (GDICT_DATABASE_CHOOSER (window->db_chooser),
			    		window->context);
  g_signal_connect (window->db_chooser, "database-activated",
	  	    G_CALLBACK (database_activated_cb),
		    window);
  gdict_sidebar_add_page (GDICT_SIDEBAR (window->sidebar),
	  		  GDICT_SIDEBAR_DATABASES_PAGE,
			  _("Available dictionaries"),
			  window->db_chooser);
  gtk_widget_show (window->db_chooser);

  /* bind the database property to the database setting */
  g_settings_bind (window->settings, GDICT_SETTINGS_DATABASE_KEY,
                   window, "database",
                   G_SETTINGS_BIND_DEFAULT);

  /* Strategy chooser */
  if (window->context)
    gdict_strategy_chooser_set_context (GDICT_STRATEGY_CHOOSER (window->strat_chooser),
                                        window->context);
  g_signal_connect (window->strat_chooser, "strategy-activated",
                    G_CALLBACK (strategy_activated_cb),
                    window);
  gdict_sidebar_add_page (GDICT_SIDEBAR (window->sidebar),
                          GDICT_SIDEBAR_STRATEGIES_PAGE,
                          _("Available strategies"),
                          window->strat_chooser);
  gtk_widget_show (window->strat_chooser);

  /* bind the strategy property to the strategy setting */
  g_settings_bind (window->settings, GDICT_SETTINGS_STRATEGY_KEY,
                   window, "strategy",
                   G_SETTINGS_BIND_DEFAULT);

  /* Source chooser */
  window->source_chooser = gdict_source_chooser_new_with_loader (window->loader);
  g_signal_connect (window->source_chooser, "source-activated",
                    G_CALLBACK (source_activated_cb),
                    window);
  gdict_sidebar_add_page (GDICT_SIDEBAR (window->sidebar),
                          GDICT_SIDEBAR_SOURCES_PAGE,
                          _("Dictionary sources"),
                          window->source_chooser);
  gtk_widget_show (window->source_chooser);

  /* bind the source-name property to the source setting */
  g_settings_bind (window->settings, GDICT_SETTINGS_SOURCE_KEY,
                   window, "source-name",
                   G_SETTINGS_BIND_DEFAULT);

  gtk_container_add (GTK_CONTAINER (frame2), window->sidebar);

  gtk_paned_pack1 (GTK_PANED (handle), frame1, TRUE, FALSE);
  gtk_paned_pack2 (GTK_PANED (handle), frame2, FALSE, TRUE);

  window->defbox_frame = frame1;
  window->sidebar_frame = frame2;

  gtk_widget_show (window->defbox_frame);

  if (window->sidebar_visible)
    {
      GtkAction *action;

      action = gtk_action_group_get_action (window->action_group, "ViewSidebar");
      gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), TRUE);
      gtk_widget_show (window->sidebar_frame);
    }

  window->status = gtk_statusbar_new ();
  gtk_box_pack_end (GTK_BOX (window->main_box), window->status, FALSE, FALSE, 0);
  if (window->statusbar_visible)
    {
      GtkAction *action;

      action = gtk_action_group_get_action (window->action_group, "ViewStatusbar");
      gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), TRUE);
      gtk_widget_show (window->status);
    }

  window->progress = gtk_progress_bar_new ();
  gtk_box_pack_end (GTK_BOX (window->status), window->progress, FALSE, FALSE, 0);

  /* retrieve the document font size */
  font_name = g_settings_get_string (window->desktop_settings, DOCUMENT_FONT_KEY);
  gdict_window_set_defbox_font (window, font_name);
  font_desc = pango_font_description_from_string (font_name);
  g_free (font_name);

  g_settings_bind (window->desktop_settings, DOCUMENT_FONT_KEY,
                   window, "defbox-font",
                   G_SETTINGS_BIND_GET);

  /* if the (width, height) tuple is not defined, use the font to
   * calculate the right window geometry
   */
  if (window->default_width == -1 || window->default_height == -1)
    {
      gint font_size;
      gint width, height;
  
      font_size = pango_font_description_get_size (font_desc);
      font_size = PANGO_PIXELS (font_size);

      width = MAX (GDICT_WINDOW_COLUMNS * font_size, GDICT_WINDOW_MIN_WIDTH);
      height = MAX (GDICT_WINDOW_ROWS * font_size, GDICT_WINDOW_MIN_HEIGHT);

      window->default_width = width;
      window->default_height = height;
    }

  pango_font_description_free (font_desc);
  
  gtk_window_set_title (GTK_WINDOW (window), _("Dictionary"));
  gtk_window_set_default_size (GTK_WINDOW (window),
                               window->default_width,
                               window->default_height);
  if (window->is_maximized)
    gtk_window_maximize (GTK_WINDOW (window));

  gtk_widget_get_allocation (GTK_WIDGET (window), &allocation);
  gtk_paned_set_position (GTK_PANED (handle), allocation.width - window->sidebar_width);
  if (window->sidebar_page != NULL)
    gdict_sidebar_view_page (GDICT_SIDEBAR (window->sidebar), window->sidebar_page);
  else
    gdict_sidebar_view_page (GDICT_SIDEBAR (window->sidebar), GDICT_SIDEBAR_SPELLER_PAGE);

  g_signal_connect (window, "delete-event",
		    G_CALLBACK (gdict_window_delete_event_cb),
		    NULL);
  g_signal_connect (window, "window-state-event",
		    G_CALLBACK (gdict_window_state_event_cb),
		    NULL);
  g_signal_connect (handle, "notify::position",
		    G_CALLBACK (gdict_window_handle_notify_position_cb),
		    window);

  gtk_widget_grab_focus (window->entry);

  window->in_construction = FALSE;

  return object;
}

static void
gdict_window_class_init (GdictWindowClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gdict_window_properties[PROP_ACTION] =
    g_param_spec_enum ("action",
                       "Action",
                       "The default action performed by the window",
                       GDICT_TYPE_WINDOW_ACTION,
                       GDICT_WINDOW_ACTION_CLEAR,
                       G_PARAM_READWRITE |
                       G_PARAM_STATIC_STRINGS |
                       G_PARAM_CONSTRUCT_ONLY);

  gdict_window_properties[PROP_SOURCE_LOADER] =
    g_param_spec_object ("source-loader",
                         "Source Loader",
                         "The GdictSourceLoader to be used to load dictionary sources",
                         GDICT_TYPE_SOURCE_LOADER,
                         G_PARAM_READWRITE |
                         G_PARAM_STATIC_STRINGS |
                         G_PARAM_CONSTRUCT_ONLY);

  gdict_window_properties[PROP_SOURCE_NAME] =
    g_param_spec_string ("source-name",
                         "Source Name",
                         "The name of the GdictSource to be used",
                         GDICT_DEFAULT_SOURCE_NAME,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  gdict_window_properties[PROP_DATABASE] =
    g_param_spec_string ("database",
                         "Database",
                         "The name of the database to search",
                         GDICT_DEFAULT_DATABASE,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  gdict_window_properties[PROP_STRATEGY] =
    g_param_spec_string ("strategy",
                         "Strategy",
                         "The name of the strategy",
                         GDICT_DEFAULT_STRATEGY,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  gdict_window_properties[PROP_PRINT_FONT] =
    g_param_spec_string ("print-font",
                         "Print Font",
                         "The font name to be used when printing",
                         GDICT_DEFAULT_PRINT_FONT,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  gdict_window_properties[PROP_DEFBOX_FONT] =
    g_param_spec_string ("defbox-font",
                         "Defbox Font",
                         "The font name to be used by the defbox widget",
                         GDICT_DEFAULT_DEFBOX_FONT,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  gdict_window_properties[PROP_WORD] =
    g_param_spec_string ("word",
                         "Word",
                         "The word to search",
                         NULL,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  gdict_window_properties[PROP_WINDOW_ID] =
    g_param_spec_uint ("window-id",
                       "Window ID",
                       "The unique identifier for this window",
                       0, G_MAXUINT,
                       0,
                       G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  gdict_window_signals[CREATED] =
    g_signal_new ("created",
                  G_OBJECT_CLASS_TYPE (gobject_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GdictWindowClass, created),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__OBJECT,
                  G_TYPE_NONE, 1,
                  GDICT_TYPE_WINDOW);

  gobject_class->finalize = gdict_window_finalize;
  gobject_class->dispose = gdict_window_dispose;
  gobject_class->set_property = gdict_window_set_property;
  gobject_class->get_property = gdict_window_get_property;
  gobject_class->constructor = gdict_window_constructor;

  g_object_class_install_properties (gobject_class,
                                     LAST_PROP,
                                     gdict_window_properties);

  widget_class->size_allocate = gdict_window_size_allocate;
}

static void
gdict_window_init (GdictWindow *window)
{
  window->action = GDICT_WINDOW_ACTION_CLEAR;
  
  window->loader = NULL;
  window->context = NULL;

  window->settings = g_settings_new (GDICT_SETTINGS_SCHEMA);
  window->desktop_settings = g_settings_new (DESKTOP_SETTINGS_SCHEMA);

  window->word = NULL;
  window->source_name = NULL;
  window->print_font = NULL;
  window->defbox_font = NULL;

  window->database = NULL;
  window->strategy = NULL;

  window->default_width = -1;
  window->default_height = -1;
  window->is_maximized = FALSE;
  window->sidebar_visible = FALSE;
  window->statusbar_visible = FALSE;
  window->sidebar_page = NULL;
  
  window->window_id = (gulong) time (NULL);

  /* we need to create the chooser widgets for the sidebar before
   * we set the construction properties
   */
  window->db_chooser = gdict_database_chooser_new ();
  window->strat_chooser = gdict_strategy_chooser_new ();
}

GtkWidget *
gdict_window_new (GdictWindowAction  action,
		  GdictSourceLoader *loader,
		  const gchar       *source_name,
		  const gchar       *word)
{
  GtkWidget *retval;
  GdictWindow *window;
  
  g_return_val_if_fail (GDICT_IS_SOURCE_LOADER (loader), NULL);
  
  retval = g_object_new (GDICT_TYPE_WINDOW,
  			 "action", action,
                         "source-loader", loader,
			 "source-name", source_name,
			 NULL);

  window = GDICT_WINDOW (retval);

  if (word && word[0] != '\0')
    {
      switch (action)
        {
	case GDICT_WINDOW_ACTION_LOOKUP:
	  gtk_entry_set_text (GTK_ENTRY (window->entry), word);
	  gdict_window_set_word (window, word, NULL);
	  break;

	case GDICT_WINDOW_ACTION_MATCH:
          {
          GdictSource *source;
          GdictContext *context;

	  gtk_entry_set_text (GTK_ENTRY (window->entry), word);
          
          gdict_window_set_sidebar_visible (window, TRUE);
          gdict_sidebar_view_page (GDICT_SIDEBAR (window->sidebar),
                                   GDICT_SIDEBAR_SPELLER_PAGE);

          /* we clone the context, so that the signals that it
           * fires do not get caught by the signal handlers we
           * use for getting the definitions.
           */
          source = gdict_source_loader_get_source (window->loader,
                                                   window->source_name);
          context = gdict_source_get_context (source);

          gdict_speller_set_context (GDICT_SPELLER (window->speller), context);
          
          g_object_unref (context);
          g_object_unref (source);
      
          gdict_speller_set_strategy (GDICT_SPELLER (window->speller),
                                      window->strategy);
      
          gdict_speller_match (GDICT_SPELLER (window->speller), word);
          }
          break;

	case GDICT_WINDOW_ACTION_CLEAR:
          gdict_defbox_clear (GDICT_DEFBOX (window->defbox));
	  break;

	default:
	  g_assert_not_reached ();
	  break;
	}
    }

  return retval;
}

/* GdictWindowAction */
static const GEnumValue _gdict_window_action_values[] = {
  { GDICT_WINDOW_ACTION_LOOKUP, "GDICT_WINDOW_ACTION_LOOKUP", "lookup" },
  { GDICT_WINDOW_ACTION_MATCH, "GDICT_WINDOW_ACTION_MATCH", "match" },
  { GDICT_WINDOW_ACTION_CLEAR, "GDICT_WINDOW_ACTION_CLEAR", "clear" },
  { 0, NULL, NULL }
};

GType
gdict_window_action_get_type (void)
{
  static GType our_type = 0;

  if (!our_type)
    our_type = g_enum_register_static ("GdictWindowAction", _gdict_window_action_values);

  return our_type;
}
