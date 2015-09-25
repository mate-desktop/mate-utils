AM_CPPFLAGS =							\
	-I.							\
	-I$(srcdir)						\
	-DMATELOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale"\"	\
	-DUIDIR=\""$(datadir)/mate-screenshot"\"

bin_PROGRAMS = mate-screenshot

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
	screenshot-xfer.h

mate_screenshot_CFLAGS =		\
	$(GLIB_CFLAGS)			\
	$(GIO_CFLAGS)			\
	$(LIBCANBERRA_GTK_CFLAGS)	\
	$(GTHREAD_CFLAGS)		\
	$(GTK_CFLAGS)

mate_screenshot_LDFLAGS = -export-dynamic

mate_screenshot_LDADD =		\
	$(XSHAPE_LIBS)			\
	$(GLIB_LIBS)			\
	$(GIO_LIBS)			\
	$(LIBCANBERRA_GTK_LIBS)		\
	$(GTHREAD_LIBS)			\
	$(GTK_LIBS)			\
	-lm

install-exec-local:
	rm -f $(DESTDIR)$(bindir)/mate-panel-screenshot
	ln -s mate-screenshot $(DESTDIR)$(bindir)/mate-panel-screenshot

