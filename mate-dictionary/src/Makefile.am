# This file is part of MATE Utils.
#
# MATE Utils is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# MATE Utils is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with MATE Utils.  If not, see <https://www.gnu.org/licenses/>.

NULL =

AM_CPPFLAGS = \
	-DPREFIX=\""$(prefix)"\"			\
	-DSYSCONFDIR=\""$(sysconfdir)"\"		\
	-DLIBDIR=\""$(libdir)"\"			\
	-DDATADIR=\""$(datadir)"\" 			\
	-DPKGDATADIR=\""$(datadir)/mate-dictionary"\" 	\
	-DMATELOCALEDIR=\""$(datadir)/locale"\"		\
	$(NULL)

bin_PROGRAMS = mate-dictionary

if BUILD_GDICT_APPLET
if ENABLE_IN_PROCESS
pkglib_LTLIBRARIES = libmate-dictionary-applet.la
else
libexec_PROGRAMS = mate-dictionary-applet
endif
endif

mate_dictionary_SOURCES = \
	gdict-about.c		\
	gdict-about.h		\
	gdict-app.c		\
	gdict-app.h		\
	gdict-common.c 		\
	gdict-common.h		\
	gdict-pref-dialog.c	\
	gdict-pref-dialog.h	\
	gdict-print.c 		\
	gdict-print.h 		\
	gdict-sidebar.c		\
	gdict-sidebar.h		\
	gdict-source-dialog.c 	\
	gdict-source-dialog.h 	\
	gdict-window.c 		\
	gdict-window.h 		\
	main.c			\
	$(NULL)

mate_dictionary_CFLAGS = \
	-I$(top_builddir)/mate-dictionary \
	-I$(top_srcdir)/mate-dictionary \
	$(GLIB_CFLAGS) \
	$(GIO_CFLAGS) \
	$(GTK_CFLAGS) \
	$(MATE_DESKTOP_CFLAGS) \
	$(WARN_CFLAGS) \
	$(NULL)

mate_dictionary_LDADD = \
	-lm			\
	$(top_builddir)/mate-dictionary/libgdict/libmatedict.la \
	$(GLIB_LIBS) \
	$(GIO_LIBS) \
	$(GTK_LIBS) \
	$(MATE_DESKTOP_LIBS) \
	$(NULL)

if BUILD_GDICT_APPLET

if ENABLE_IN_PROCESS
libmate_dictionary_applet_la_SOURCES = \
	gdict-about.c 		\
	gdict-about.h		\
	gdict-aligned-window.c	\
	gdict-aligned-window.h	\
	gdict-applet.c		\
	gdict-applet.h		\
	gdict-common.c		\
	gdict-common.h		\
	gdict-pref-dialog.c	\
	gdict-pref-dialog.h	\
	gdict-print.c 		\
	gdict-print.h 		\
	gdict-sidebar.c		\
	gdict-sidebar.h		\
	gdict-source-dialog.c 	\
	gdict-source-dialog.h 	\
	$(NULL)

libmate_dictionary_applet_la_CFLAGS = \
	-I$(top_builddir)/mate-dictionary \
	-I$(top_srcdir)/mate-dictionary \
	$(APPLET_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(GIO_CFLAGS) \
	$(GTK_CFLAGS) \
	$(MATE_DESKTOP_CFLAGS) \
	$(WARN_CFLAGS) \
	$(NULL)

libmate_dictionary_applet_la_LIBADD = \
	-lm			\
	$(top_builddir)/mate-dictionary/libgdict/libmatedict.la \
	$(APPLET_LIBS) \
	$(GLIB_LIBS) \
	$(GIO_LIBS) \
	$(GTK_LIBS) \
	$(MATE_DESKTOP_LIBS) \
	$(NULL)

if ENABLE_WAYLAND
libmate_dictionary_applet_la_LIBADD += \
	$(WAYLAND_LIBS)
libmate_dictionary_applet_la_CFLAGS += \
	$(WAYLAND_CFLAGS)
endif

else
mate_dictionary_applet_SOURCES = \
	gdict-about.c 		\
	gdict-about.h		\
	gdict-aligned-window.c	\
	gdict-aligned-window.h	\
	gdict-applet.c		\
	gdict-applet.h		\
	gdict-common.c		\
	gdict-common.h		\
	gdict-pref-dialog.c	\
	gdict-pref-dialog.h	\
	gdict-print.c 		\
	gdict-print.h 		\
	gdict-sidebar.c		\
	gdict-sidebar.h		\
	gdict-source-dialog.c 	\
	gdict-source-dialog.h 	\
	$(NULL)

mate_dictionary_applet_CFLAGS = \
	-I$(top_builddir)/mate-dictionary \
	-I$(top_srcdir)/mate-dictionary \
	$(APPLET_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(GIO_CFLAGS) \
	$(GTK_CFLAGS) \
	$(MATE_DESKTOP_CFLAGS) \
	$(WARN_CFLAGS) \
	$(NULL)

mate_dictionary_applet_LDADD = \
	-lm			\
	$(top_builddir)/mate-dictionary/libgdict/libmatedict.la \
	$(APPLET_LIBS) \
	$(GLIB_LIBS) \
	$(GIO_LIBS) \
	$(GTK_LIBS) \
	$(MATE_DESKTOP_LIBS) \
	$(NULL)
endif # ENABLE_IN_PROCESS
endif # BUILD_GDICT_APPLET

-include $(top_srcdir)/git.mk
