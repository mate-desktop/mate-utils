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

static char *
make_temp_directory (void)
{
  gchar *dir_name;

  // mkdtemp uses XXXXXX as a template to create a unique name for the temp dir
  dir_name = g_build_filename (g_get_tmp_dir (),
                               "mate-screenshot.XXXXXX",
                               NULL);

  if (mkdtemp (dir_name) == NULL)
    {
      g_warning ("Failed to create temporary directory: %s", g_strerror (errno));
      g_free (dir_name);
      return NULL;
    }

  return dir_name;
}

static void
cleanup_handler (void)
{
  clean_up_temporary_dir ();
}

void
screenshot_save_start (GdkPixbuf    *pixbuf,
                       SaveFunction  callback,
                       gpointer      user_data)
{
  GError *error = NULL;

  static gboolean cleanup_registered = FALSE;
  if (!cleanup_registered)
    {
      atexit (cleanup_handler);
      cleanup_registered = TRUE;
    }

  clean_up_temporary_dir ();

  parent_dir = make_temp_directory ();
  if (parent_dir == NULL)
    {
      if (callback)
        callback (user_data);
      return;
    }

  tmp_filename = g_build_filename (parent_dir,
                                   _("Screenshot.png"),
                                   NULL);

  if (! gdk_pixbuf_save (pixbuf, tmp_filename,
                        "png", &error,
                        "tEXt::Software", "mate-screenshot",
                        NULL))
    {
      if (error && error->message)
        g_warning ("Failed to save screenshot: %s", error->message);
      else
        g_warning ("Failed to save screenshot: Unknown error");

      g_error_free (error);

      clean_up_temporary_dir ();
    }

  if (callback)
    callback (user_data);
}

const char *
screenshot_save_get_filename (void)
{
  return tmp_filename;
}
