/* Copyright (C) 2001-2006 Jonathan Blandford <jrb@alum.mit.edu>
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <glib/gi18n.h>

#include "screenshot-save.h"

typedef struct
{
  GdkPixbuf          *pixbuf;
  GFileOutputStream  *stream;
  SaveFunction        callback;
  gpointer            user_data;
}
SaveAsyncData;

static char *parent_dir = NULL;
static char *tmp_filename = NULL;

static void
clean_up_temporary_dir (void)
{
  char *message;
  gboolean error_occurred = FALSE;
  if (tmp_filename && g_file_test (tmp_filename, G_FILE_TEST_EXISTS))
    error_occurred = unlink (tmp_filename);
  if (parent_dir && g_file_test (parent_dir, G_FILE_TEST_EXISTS))
    error_occurred = rmdir (parent_dir) || error_occurred;

  if (error_occurred)
    {
      message = g_strdup_printf (_("Unable to clear the temporary folder:\n%s"),
				 tmp_filename);
      g_warning ("%s", message);
      g_free (message);
    }
  g_free (tmp_filename);
  g_free (parent_dir);
  tmp_filename = NULL;
  parent_dir = NULL;
}

static void
cleanup_handler (void)
{
  clean_up_temporary_dir ();
}

static void
save_async_finish_and_free (SaveAsyncData *data, GError *err)
{
  if (err)
    {
      GtkWidget *dialog;

      dialog = gtk_message_dialog_new (NULL, 0, GTK_MESSAGE_ERROR,
                                       GTK_BUTTONS_OK,
                                       "Unable to save the screenshot to disk:\n\n%s",
                                       err->message);
      gtk_dialog_run (GTK_DIALOG (dialog));
      gtk_widget_destroy (dialog);

      clean_up_temporary_dir ();
    }

  if (data->callback)
    data->callback (data->user_data);

  if (data->stream)
    g_clear_object (&data->stream);

  g_clear_object (&data->pixbuf);
  g_free (data);
}

static void
stream_close_async_ready (GObject* object, GAsyncResult* res, gpointer user_data)
{
  SaveAsyncData  *data  = user_data;
  GError         *err   = NULL;

  if (! g_output_stream_close_finish (G_OUTPUT_STREAM(object), res, &err))
    {
      save_async_finish_and_free (data, err);
      g_error_free (err);
    }
  else
    {
      /* all done! */
      save_async_finish_and_free (data, NULL);
    }
}

static void
save_to_stream_async_ready (GObject* object, GAsyncResult* res, gpointer user_data)
{
  SaveAsyncData  *data  = user_data;
  GError         *err   = NULL;

  if (! gdk_pixbuf_save_to_stream_finish (res, &err))
    {
      save_async_finish_and_free (data, err);
      g_error_free (err);
    }
  else
    {
      g_output_stream_close_async (G_OUTPUT_STREAM (data->stream), G_PRIORITY_DEFAULT,
                                   NULL, stream_close_async_ready, data);
    }
}

static void
replace_async_ready (GObject* object, GAsyncResult* res, gpointer user_data)
{
  SaveAsyncData  *data  = user_data;
  GError         *err   = NULL;

  data->stream = g_file_replace_finish (G_FILE(object), res, &err);
  if (! data->stream)
    {
      save_async_finish_and_free (data, err);
      g_error_free (err);
    }
  else
    {
      gdk_pixbuf_save_to_stream_async (data->pixbuf, G_OUTPUT_STREAM (data->stream),
                                       "png", NULL, save_to_stream_async_ready, data,
                                       "tEXt::Software", "mate-screenshot",
                                       NULL);
    }
}

#if ! GLIB_CHECK_VERSION (2, 74, 0)
/* poor man's emulation of the temp dir async API -- it's not actually async */
static void
file_new_tmp_dir_async (const char          *tmpl,
                        int                  io_priority,
                        GCancellable        *cancellable,
                        GAsyncReadyCallback  callback,
                        gpointer             user_data)
{
  GError *error = NULL;
  gchar *path;
  gpointer task_data[2];

  path = g_dir_make_tmp (tmpl, &error);
  task_data[0] = path;
  task_data[1] = error;
  callback (NULL, (GAsyncResult *) task_data, user_data);
  if (path)
    g_free (path);
  /* error is freed in finish() */
}

static GFile *
file_new_tmp_dir_finish (GAsyncResult  *result,
                         GError       **error)
{
  gpointer *task_data = (gpointer *) result;

  if (task_data[0])
    return g_file_new_for_path (task_data[0]);

  g_propagate_error (error, task_data[1]);

  return NULL;
}

#define g_file_new_tmp_dir_async file_new_tmp_dir_async
#define g_file_new_tmp_dir_finish file_new_tmp_dir_finish
#endif

/* gets a #GFile path, setting @err if there is none */
static gchar *
get_path (GFile *file, GError **err)
{
  gchar *path;

  path = g_file_get_path (file);
  if (! path)
    {
      gchar *uri = g_file_get_uri (file);

      g_set_error (err, G_FILE_ERROR, G_FILE_ERROR_NOENT,
                   _("Temporary file \"%s\" does not have a path"), uri);
      g_free (uri);
    }

  return path;
}

static void
new_tmp_dir_async_ready (GObject* object, GAsyncResult* res, gpointer user_data)
{
  SaveAsyncData  *data  = user_data;
  GError         *err   = NULL;

  GFile *dir = g_file_new_tmp_dir_finish (res, &err);
  if (dir)
    {
      g_free (parent_dir);
      parent_dir = get_path (dir, &err);
      if (parent_dir)
        {
          GFile *file = g_file_get_child (dir, _("Screenshot.png"));

          g_free (tmp_filename);
          tmp_filename = get_path (file, &err);
          if (tmp_filename)
            {
              g_file_replace_async (file, NULL, FALSE, G_FILE_CREATE_REPLACE_DESTINATION,
                                    G_PRIORITY_DEFAULT, NULL, replace_async_ready, data);
            }

          g_object_unref (file);
        }

      g_object_unref (dir);
    }

  /* error will be set if anything failed, so we didn't schedule the next
   * asynchronous call */
  if (err)
    {
      save_async_finish_and_free (data, err);
      g_error_free (err);
    }
}

void
screenshot_save_start (GdkPixbuf    *pixbuf,
                       SaveFunction  callback,
                       gpointer      user_data)
{
  SaveAsyncData  *data;

  static gboolean cleanup_registered = FALSE;
  if (!cleanup_registered)
    {
      atexit (cleanup_handler);
      cleanup_registered = TRUE;
    }

  clean_up_temporary_dir ();

  data = g_malloc (sizeof *data);
  data->pixbuf = g_object_ref (pixbuf);
  data->stream = NULL;
  data->callback = callback;
  data->user_data = user_data;

  g_file_new_tmp_dir_async ("mate-screenshot.XXXXXX", G_PRIORITY_DEFAULT, NULL,
                            new_tmp_dir_async_ready, data);
}

const char *
screenshot_save_get_filename (void)
{
  return tmp_filename;
}
