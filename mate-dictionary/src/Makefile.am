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
libexec_PROGRAMS = mate-dictionary-applet
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
	$(WARN_CFLAGS) \
	$(NULL)

mate_dictionary_LDADD = \
	-lm			\
	$(top_builddir)/mate-dictionary/libgdict/libmatedict.la \
	$(GLIB_LIBS) \
	$(GIO_LIBS) \
	$(GTK_LIBS) \
	$(NULL)

if BUILD_GDICT_APPLET

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
	$(WARN_CFLAGS) \
	$(NULL)

mate_dictionary_applet_LDADD = \
	-lm			\
	$(top_builddir)/mate-dictionary/libgdict/libmatedict.la \
	$(APPLET_LIBS) \
	$(GLIB_LIBS) \
	$(GIO_LIBS) \
	$(GTK_LIBS) \
	$(NULL)

endif # BUILD_GDICT_APPLET
