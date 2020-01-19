NULL =

AM_CPPFLAGS =						\
	-I.						\
	-I$(srcdir)					\
	-DMATELOCALEDIR=\""$(datadir)/locale"\"		\
	$(NULL)

bin_PROGRAMS = mate-screenshot

BUILT_SOURCES =				\
	screenshot-resources.c		\
	screenshot-resources.h		\
	$(NULL)

nodist_mate_screenshot_SOURCES =	\
	$(BUILT_SOURCES)		\
	$(NULL)

mate_screenshot_SOURCES =		\
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
	$(GTK_CFLAGS)			\
	$(WARN_CFLAGS)			\
	$(NULL)

mate_screenshot_LDFLAGS = -export-dynamic

mate_screenshot_LDADD =			\
	$(XSHAPE_LIBS)			\
	$(GLIB_LIBS)			\
	$(GIO_LIBS)			\
	$(LIBCANBERRA_GTK_LIBS)		\
	$(GTHREAD_LIBS)			\
	$(GTK_LIBS)			\
	-lm

screenshot-resources.h screenshot-resources.c: $(srcdir)/../data/org.mate.screenshot.gresource.xml Makefile $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir $(srcdir)/../data $(srcdir)/../data/org.mate.screenshot.gresource.xml)
	$(AM_V_GEN) XMLLINT=$(XMLLINT) $(GLIB_COMPILE_RESOURCES) --target $@ --sourcedir $(srcdir)/../data --generate --c-name screenshot $<

CLEANFILES =				\
	$(BUILT_SOURCES)		\
	$(NULL)

install-exec-local:
	rm -f $(DESTDIR)$(bindir)/mate-panel-screenshot
	ln -s mate-screenshot $(DESTDIR)$(bindir)/mate-panel-screenshot

