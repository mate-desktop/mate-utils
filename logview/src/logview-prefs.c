/* -*- Mode: C; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2; -*- */
/* logview-prefs.c - logview user preferences handling
 *
 * Copyright (C) 1998  Cesar Miquel  <miquel@df.uba.ar>
 * Copyright (C) 2004  Vincent Noel
 * Copyright (C) 2006  Emmanuele Bassi
 * Copyright (C) 2008  Cosimo Cecchi <cosimoc@gnome.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/stat.h>
#include <string.h>
#include <gtk/gtk.h>

#include "logview-prefs.h"

#define LOGVIEW_DEFAULT_HEIGHT 400
#define LOGVIEW_DEFAULT_WIDTH 600

/* logview settings */
#define LOGVIEW_SCHEMA "org.mate.system-log"
#define PREF_WIDTH    "width"
#define PREF_HEIGHT	  "height"
#define PREF_LOGFILE 	"logfile"
#define PREF_LOGFILES "logfiles"
#define PREF_FONTSIZE "fontsize"
#define PREF_FILTERS  "filters"

/* desktop-wide settings */
#define MATE_MONOSPACE_FONT_NAME "monospace-font-name"
#define MATE_MENUS_HAVE_TEAROFF  "menus-have-tearoff"

static LogviewPrefs *singleton = NULL;

enum {
  SYSTEM_FONT_CHANGED,
  HAVE_TEAROFF_CHANGED,
  LAST_SIGNAL
};

enum {
  FILTER_NAME,
  FILTER_INVISIBLE,
  FILTER_FOREGROUND,
  FILTER_BACKGROUND,
  FILTER_REGEX,
  MAX_TOKENS
};

static guint signals[LAST_SIGNAL] = { 0 };

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), LOGVIEW_TYPE_PREFS, LogviewPrefsPrivate))

struct _LogviewPrefsPrivate {
  GSettings *logview_prefs;
  GSettings *interface_prefs;

  GHashTable *filters;
};

G_DEFINE_TYPE (LogviewPrefs, logview_prefs, G_TYPE_OBJECT);

static void
do_finalize (GObject *obj)
{
  LogviewPrefs *prefs = LOGVIEW_PREFS (obj);

  g_hash_table_destroy (prefs->priv->filters);

  g_object_unref (prefs->priv->logview_prefs);
  g_object_unref (prefs->priv->interface_prefs);

  G_OBJECT_CLASS (logview_prefs_parent_class)->finalize (obj);
}

static void
logview_prefs_class_init (LogviewPrefsClass *klass)
{
  GObjectClass *oclass = G_OBJECT_CLASS (klass);

  oclass->finalize = do_finalize;

  signals[SYSTEM_FONT_CHANGED] = g_signal_new ("system-font-changed",
                                               G_OBJECT_CLASS_TYPE (oclass),
                                               G_SIGNAL_RUN_LAST,
                                               G_STRUCT_OFFSET (LogviewPrefsClass, system_font_changed),
                                               NULL, NULL,
                                               g_cclosure_marshal_VOID__STRING,
                                               G_TYPE_NONE, 1,
                                               G_TYPE_STRING);
  signals[HAVE_TEAROFF_CHANGED] = g_signal_new ("have-tearoff-changed",
                                                G_OBJECT_CLASS_TYPE (oclass),
                                                G_SIGNAL_RUN_LAST,
                                                G_STRUCT_OFFSET (LogviewPrefsClass, have_tearoff_changed),
                                                NULL, NULL,
                                                g_cclosure_marshal_VOID__BOOLEAN,
                                                G_TYPE_NONE, 1,
                                                G_TYPE_BOOLEAN);

  g_type_class_add_private (klass, sizeof (LogviewPrefsPrivate));
}

static void
have_tearoff_changed_cb (GSettings *settings,
                         gchar *key,
                         gpointer data)
{
  LogviewPrefs *prefs = data;
  gboolean add_tearoffs;

  add_tearoffs = g_settings_get_boolean (settings, key);
  g_signal_emit (prefs, signals[HAVE_TEAROFF_CHANGED], 0, add_tearoffs, NULL);
}

static void
monospace_font_changed_cb (GSettings *settings,
                           gchar *key,
                           gpointer data)
{
  LogviewPrefs *prefs = data;
  gchar *monospace_font_name;

  monospace_font_name = g_settings_get_string (settings, key);
  g_signal_emit (prefs, signals[SYSTEM_FONT_CHANGED], 0, monospace_font_name, NULL);

  g_free (monospace_font_name);
}

#define DELIMITER ":"

static void
load_filters (LogviewPrefs *prefs)
{
  gchar **filters;
  gchar **tokens;
  const gchar *str;
  LogviewFilter *filter;
  GtkTextTag *tag;
  GdkRGBA color;
  gint idx;

  filters = g_settings_get_strv (prefs->priv->logview_prefs,
                                 PREF_FILTERS);

  prefs->priv->filters = g_hash_table_new_full (g_str_hash, g_str_equal,
                                                (GDestroyNotify) g_free,
                                                (GDestroyNotify) g_object_unref);

  for (idx = 0; filters[idx] != NULL; idx++) {
    str = filters[idx];
    tokens = g_strsplit (str, DELIMITER, MAX_TOKENS);
    filter = logview_filter_new (tokens[FILTER_NAME], tokens[FILTER_REGEX]);
    tag = gtk_text_tag_new (tokens[FILTER_NAME]);

    g_object_set (tag, "invisible",
                  g_str_equal (tokens[FILTER_INVISIBLE], "1"), NULL);

    if (strlen (tokens[FILTER_FOREGROUND])) {
      gdk_rgba_parse (&color, tokens[FILTER_FOREGROUND]);
      g_object_set (tag, "foreground-rgba", &color,
                    "foreground-set", TRUE, NULL);
    }

    if (strlen (tokens[FILTER_BACKGROUND])) {
      gdk_rgba_parse (&color, tokens[FILTER_BACKGROUND]);
      g_object_set (tag, "paragraph-background-rgba", &color,
                    "paragraph-background-set", TRUE, NULL);
    }

    g_object_set (filter, "texttag", tag, NULL);
    g_hash_table_insert (prefs->priv->filters, 
                         g_strdup(tokens[FILTER_NAME]), 
                         filter);

    g_object_ref (filter);
    g_object_unref (tag);
    g_strfreev (tokens);
  }

  g_strfreev (filters);
}

static void
save_filter_foreach_func (gpointer key, gpointer value, gpointer user_data)
{
  GPtrArray *filters;
  const gchar *name;
  LogviewFilter *filter;
  GdkRGBA  *foreground;
  gboolean foreground_set;
  GdkRGBA  *background;
  gboolean background_set;
  gchar *regex, *color;
  gboolean invisible;
  GtkTextTag *tag;
  GString *prefs_string;

  filters = user_data;
  filter = LOGVIEW_FILTER (value);
  name = key;
  color = NULL;

  prefs_string = g_string_new (name);
  g_string_append (prefs_string, DELIMITER);

  g_object_get (filter,
                "regex", &regex,
                "texttag", &tag,
                NULL);
  g_object_get (tag,
                "foreground-set", &foreground_set,
                "foreground-rgba", &foreground,
                "paragraph-background-set", &background_set,
                "paragraph-background-rgba", &background,
                "invisible", &invisible, NULL);

  if (invisible) {
    g_string_append (prefs_string, "1" DELIMITER);
  } else {
    g_string_append (prefs_string, "0" DELIMITER);
  }

  if (foreground_set) {
    color = gdk_rgba_to_string (foreground);
    g_string_append (prefs_string, color);
    g_free (color);
  }

  if (foreground) {
    gdk_rgba_free (foreground);
  }

  g_string_append (prefs_string, DELIMITER);

  if (background_set) {
    color = gdk_rgba_to_string (background);
    g_string_append (prefs_string, color);
    g_free (color);
  }

  if (background) {
    gdk_rgba_free (background);
  }

  g_string_append (prefs_string, DELIMITER);
  g_string_append (prefs_string, regex);

  g_free (regex);
  g_object_unref (tag);
  
  g_ptr_array_add (filters, g_string_free (prefs_string, FALSE));
}

static void
save_filters (LogviewPrefs *prefs)
{
  GPtrArray *filters;
  gchar **filters_strv;

  filters = g_ptr_array_new ();
  g_hash_table_foreach (prefs->priv->filters,
                        save_filter_foreach_func,
                        filters);
  g_ptr_array_add (filters, NULL);

  filters_strv = (gchar **) g_ptr_array_free (filters, FALSE);
  g_settings_set_strv (prefs->priv->logview_prefs,
                       PREF_FILTERS,
                       (const gchar **) filters_strv);

  g_strfreev (filters_strv);
}

static void
get_filters_foreach (gpointer key, gpointer value, gpointer user_data)
{
  GList **list;
  list = user_data;
  *list = g_list_append (*list, value);
}

static void
logview_prefs_init (LogviewPrefs *self)
{
  LogviewPrefsPrivate *priv;

  priv = self->priv = GET_PRIVATE (self);

  priv->logview_prefs = g_settings_new (LOGVIEW_SCHEMA);
  priv->interface_prefs = g_settings_new ("org.mate.interface");

  g_signal_connect (priv->interface_prefs, "changed::" MATE_MONOSPACE_FONT_NAME,
                    G_CALLBACK (monospace_font_changed_cb), self);
  g_signal_connect (priv->interface_prefs, "changed::" MATE_MENUS_HAVE_TEAROFF,
                    G_CALLBACK (have_tearoff_changed_cb), self);

  load_filters (self);
}

/* public methods */

LogviewPrefs *
logview_prefs_get ()
{
  if (!singleton)
    singleton = g_object_new (LOGVIEW_TYPE_PREFS, NULL);

  return singleton;
}

void
logview_prefs_store_window_size (LogviewPrefs *prefs,
                                 int width, int height)
{
  g_assert (LOGVIEW_IS_PREFS (prefs));

  g_settings_set_int (prefs->priv->logview_prefs,
                      PREF_WIDTH, width);
  g_settings_set_int (prefs->priv->logview_prefs,
                      PREF_HEIGHT, height);
}

void
logview_prefs_get_stored_window_size (LogviewPrefs *prefs,
                                      int *width, int *height)
{
  g_assert (LOGVIEW_IS_PREFS (prefs));

  *width = g_settings_get_int (prefs->priv->logview_prefs,
                               PREF_WIDTH);
  *height = g_settings_get_int (prefs->priv->logview_prefs,
                                PREF_HEIGHT);

  if ((*width == 0) ^ (*height == 0)) {
    /* if one of the two failed, return default for both */
    *width = LOGVIEW_DEFAULT_WIDTH;
    *height = LOGVIEW_DEFAULT_HEIGHT;
  }
}

char *
logview_prefs_get_monospace_font_name (LogviewPrefs *prefs)
{
  g_assert (LOGVIEW_IS_PREFS (prefs));

  return (g_settings_get_string (prefs->priv->interface_prefs, MATE_MONOSPACE_FONT_NAME));
}

gboolean
logview_prefs_get_have_tearoff (LogviewPrefs *prefs)
{
  g_assert (LOGVIEW_IS_PREFS (prefs));

	return (g_settings_get_boolean (prefs->priv->interface_prefs, MATE_MENUS_HAVE_TEAROFF));
}

/* the elements should be freed with g_free () */

gchar **
logview_prefs_get_stored_logfiles (LogviewPrefs *prefs)
{
  g_assert (LOGVIEW_IS_PREFS (prefs));

  return g_settings_get_strv (prefs->priv->logview_prefs,
                              PREF_LOGFILES);
}

void
logview_prefs_store_log (LogviewPrefs *prefs, GFile *file)
{
  gchar **stored_logs;
  GFile *stored;
  gboolean found = FALSE;
  gint idx, old_size;

  g_assert (LOGVIEW_IS_PREFS (prefs));
  g_assert (G_IS_FILE (file));

  stored_logs = logview_prefs_get_stored_logfiles (prefs);

  for (idx = 0; stored_logs[idx] != NULL; idx++) {
    stored = g_file_parse_name (stored_logs[idx]);
    if (g_file_equal (file, stored)) {
      found = TRUE;
    }

    g_object_unref (stored);

    if (found) {
      break;
    }
  }

  if (!found) {
    old_size = g_strv_length (stored_logs);
    stored_logs = g_realloc (stored_logs, (old_size + 2) * sizeof (gchar *));
    stored_logs[old_size] = g_file_get_parse_name (file);
    stored_logs[old_size + 1] = NULL;

    g_settings_set_strv (prefs->priv->logview_prefs,
                         PREF_LOGFILES,
                         (const gchar **) stored_logs);
  }

  g_strfreev (stored_logs);
}

void
logview_prefs_remove_stored_log (LogviewPrefs *prefs, GFile *target)
{
  gchar **stored_logs;
  GFile *stored;
  GPtrArray *new_value;
  gint idx;
  gboolean removed = FALSE;

  g_assert (LOGVIEW_IS_PREFS (prefs));
  g_assert (G_IS_FILE (target));

  stored_logs = logview_prefs_get_stored_logfiles (prefs);
  new_value = g_ptr_array_new ();

  for (idx = 0; stored_logs[idx] != NULL; idx++) {
    stored = g_file_parse_name (stored_logs[idx]);
    if (!g_file_equal (stored, target)) {
      g_ptr_array_add (new_value, g_strdup (stored_logs[idx]));
    }

    g_object_unref (stored);
  }

  g_ptr_array_add (new_value, NULL);
  g_strfreev (stored_logs);
  stored_logs = (gchar **) g_ptr_array_free (new_value, FALSE);

  g_settings_set_strv (prefs->priv->logview_prefs,
                       PREF_LOGFILES,
                       (const gchar **) stored_logs);

  g_strfreev (stored_logs);
}

void
logview_prefs_store_fontsize (LogviewPrefs *prefs, int fontsize)
{
  g_assert (LOGVIEW_IS_PREFS (prefs));
  g_assert (fontsize > 0);

  g_settings_set_int (prefs->priv->logview_prefs, PREF_FONTSIZE, fontsize);
}

int
logview_prefs_get_stored_fontsize (LogviewPrefs *prefs)
{
  g_assert (LOGVIEW_IS_PREFS (prefs));

	return g_settings_get_int (prefs->priv->logview_prefs, PREF_FONTSIZE);
}

void
logview_prefs_store_active_logfile (LogviewPrefs *prefs,
                                    const char *filename)
{
  g_assert (LOGVIEW_IS_PREFS (prefs));

  g_settings_set_string (prefs->priv->logview_prefs,
                         PREF_LOGFILE, filename);
}

char *
logview_prefs_get_active_logfile (LogviewPrefs *prefs)
{
  g_assert (LOGVIEW_IS_PREFS (prefs));

  return g_settings_get_string (prefs->priv->logview_prefs,
                                PREF_LOGFILE);
}

GList *
logview_prefs_get_filters (LogviewPrefs *prefs)
{
  GList *filters = NULL;

  g_assert (LOGVIEW_IS_PREFS (prefs));

  g_hash_table_foreach (prefs->priv->filters,
                        get_filters_foreach,
                        &filters);

  return filters;
}

void
logview_prefs_remove_filter (LogviewPrefs *prefs,
                             const gchar *name)
{
  g_assert (LOGVIEW_IS_PREFS (prefs));

  g_hash_table_remove (prefs->priv->filters,
                       name);

  save_filters (prefs);
}

void
logview_prefs_add_filter (LogviewPrefs *prefs,
                          LogviewFilter *filter)
{
  gchar* name;

  g_assert (LOGVIEW_IS_PREFS (prefs));
  g_assert (LOGVIEW_IS_FILTER (filter));

  g_object_get (filter, "name", &name, NULL);
  g_hash_table_insert (prefs->priv->filters, name, g_object_ref (filter));

  save_filters (prefs);
}

LogviewFilter *
logview_prefs_get_filter (LogviewPrefs *prefs,
                          const gchar *name)
{
  g_assert (LOGVIEW_IS_PREFS (prefs));

  return g_hash_table_lookup (prefs->priv->filters, name);
}

