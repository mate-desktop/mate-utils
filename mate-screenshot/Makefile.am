NULL =

AM_CPPFLAGS =							\
	-I.							\
	-I$(srcdir)						\
	-DMATELOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale"\"	\
	-DUIDIR=\""$(uidir)"\"					\
	$(NULL)

bin_PROGRAMS = \
	mate-screenshot		\
	$(NULL)

mate_screenshot_SOURCES = \
	mate-screenshot.c		\
	screenshot-dialog.c		\
	screenshot-dialog.h		\
	screenshot-shadow.c		\
	screenshot-shadow.h		\
	screenshot-utils.c		\
	screenshot-utils.h		\
	screenshot-save.c		\
	screenshot-save.h		\
	screenshot-xfer.c		\
	screenshot-xfer.h		\
	$(NULL)

mate_screenshot_CFLAGS =		\
	$(GLIB_CFLAGS)			\
	$(GIO_CFLAGS)			\
	$(LIBCANBERRA_GTK_CFLAGS)	\
	$(GTHREAD_CFLAGS)		\
	$(GTK_CFLAGS)			\
	$(NULL)

mate_screenshot_LDFLAGS = -export-dynamic

mate_screenshot_LDADD =		\
	$(XSHAPE_LIBS)			\
	$(GLIB_LIBS)			\
	$(GIO_LIBS)			\
	$(LIBCANBERRA_GTK_LIBS)		\
	$(GTHREAD_LIBS)			\
	$(GTK_LIBS)			\
	-lm				\
	$(NULL)

mate_screenshotdir  = $(datadir)/applications
mate_screenshot_in_files = mate-screenshot.desktop.in
mate_screenshot_DATA = $(mate_screenshot_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

man_MANS = mate-screenshot.1

uidir = $(datadir)/mate-screenshot
ui_DATA =					\
	mate-screenshot.ui			\
	$(NULL)

gsettingsschema_in_files = org.mate.screenshot.gschema.xml.in
gsettings_SCHEMAS = $(gsettingsschema_in_files:.xml.in=.xml)
.PRECIOUS: $(gsettings_SCHEMAS)

@INTLTOOL_XML_NOMERGE_RULE@

@GSETTINGS_RULES@

convertdir = $(datadir)/MateConf/gsettings
convert_DATA = mate-screenshot.convert

EXTRA_DIST =					\
	$(gsettingsschema_in_files)		\
	$(mate_screenshot_in_files)		\
	$(man_MANS)				\
	$(ui_DATA)				\
	$(convert_DATA)			\
	$(NULL)

CLEANFILES = \
	$(BUILT_SOURCES)			 \
	$(gsettings_SCHEMAS)			 \
	$(mate_screenshot_DATA)		 \
	$(sys_DATA)

dist-hook:
	cd $(distdir) ; rm -f $(CLEANFILES)

install-exec-local:
	rm -f $(DESTDIR)$(bindir)/mate-panel-screenshot
	ln -s mate-screenshot $(DESTDIR)$(bindir)/mate-panel-screenshot
