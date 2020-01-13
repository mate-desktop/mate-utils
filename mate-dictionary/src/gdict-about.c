/* gdict-about.c - GtkAboutDialog wrapper
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

#include <glib/gi18n.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "gdict-about.h"

void
gdict_show_about_dialog (GtkWidget *parent)
{
  const gchar *authors[] = {
    "Mike Hughes <mfh@psilord.com>",
    "Spiros Papadimitriou <spapadim+@cs.cmu.edu>",
    "Bradford Hovinen <hovinen@udel.edu>",
    "Vincent Noel <vnoel@cox.net>",
    "Emmanuele Bassi <ebassi@gmail.com>",
    NULL
  };

  const gchar *documenters[] = {
    N_("Sun GNOME Documentation Team <gdocteam@sun.com>"),
    "John Fleck <jfleck@inkstain.net>",
    "Emmanuele Bassi <ebassi@gmail.com>",
    N_("MATE Documentation Team"),
    NULL
  };

  const gchar *translator_credits = _("translator-credits");
  const gchar *comments = _("Look up words in dictionaries");

  const gchar* license[] = {
    N_("This program is free software; you can redistribute it and/or "
       "modify it under the terms of the GNU General Public License as "
       "published by the Free Software Foundation; either version 2 of "
       "the License, or (at your option) any later version."),

    N_("This program is distributed in the hope that it will be useful, "
       "but WITHOUT ANY WARRANTY; without even the implied warranty of "
       "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU "
       "General Public License for more details."),

    N_("You should have received a copy of the GNU General Public License "
       "along with this program; if not, write to the Free Software "
       "Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA "
       "02110-1301, USA.")
  };

  g_return_if_fail (GTK_IS_WIDGET (parent));

  char *license_trans = g_strjoin ("\n\n", _(license[0]), _(license[1]), _(license[2]), NULL);

#ifdef ENABLE_NLS
  const char **p;
  for (p = documenters; *p; ++p)
    *p = _(*p);
#endif

  gtk_show_about_dialog (GTK_IS_WINDOW (parent) ? GTK_WINDOW (parent) : NULL,
			 "program-name", _("Dictionary"),
  			 "version", VERSION,
			 "title", _("About Dictionary"),
			 "copyright", _("Copyright \xc2\xa9 2005-2006 Emmanuele Bassi\n"
			                "Copyright \xc2\xa9 2011-2020 MATE developers"),
  			 "comments", comments,
  			 "authors", authors,
  			 "documenters", documenters,
  			 "translator-credits", translator_credits,
  			 "logo-icon-name", "accessories-dictionary",
			 "license", license_trans,
  			 "wrap-license", TRUE,
			 "screen", gtk_widget_get_screen (parent),
  			 NULL);

  g_free(license_trans);
}
