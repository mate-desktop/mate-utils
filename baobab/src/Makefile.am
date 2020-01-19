NULL =

AM_CPPFLAGS = \
	-DPREFIX=\""$(prefix)"\" \
	-DSYSCONFDIR=\""$(sysconfdir)"\" \
	-DLIBDIR=\""$(libdir)"\" \
	-DDATADIR=\""$(datadir)"\" \
	-DBAOBAB_PIX_DIR=\""$(datadir)/mate-disk-usage-analyzer/pixmaps/"\" \
	-DMATELOCALEDIR=\""$(datadir)/locale"\" \
	$(NULL)

bin_PROGRAMS = mate-disk-usage-analyzer

BUILT_SOURCES = baobab-resources.h baobab-resources.c

baobab-resources.h baobab-resources.c: $(srcdir)/../data/org.mate.disk-usage-analyzer.gresource.xml Makefile $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies --sourcedir $(srcdir)/../data $(srcdir)/../data/org.mate.disk-usage-analyzer.gresource.xml)
	$(AM_V_GEN) XMLLINT=$(XMLLINT) $(GLIB_COMPILE_RESOURCES) --target $@ --sourcedir $(srcdir)/../data --generate --c-name baobab $<

nodist_mate_disk_usage_analyzer_SOURCES = \
	$(BUILT_SOURCES)
mate_disk_usage_analyzer_SOURCES = \
	baobab.c \
	baobab.h \
	baobab-cell-renderer-progress.c \
	baobab-cell-renderer-progress.h \
	baobab-ringschart.c \
	baobab-ringschart.h \
	baobab-scan.c \
	baobab-scan.h \
	baobab-treeview.c \
	baobab-treeview.h \
	baobab-utils.c \
	baobab-utils.h \
	callbacks.c \
	callbacks.h \
	baobab-prefs.c \
	baobab-prefs.h \
	baobab-remote-connect-dialog.c \
	baobab-remote-connect-dialog.h \
	baobab-chart.c \
	baobab-chart.h \
	baobab-treemap.c \
	baobab-treemap.h

mate_disk_usage_analyzer_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(GIO_CFLAGS) \
	$(GTK_CFLAGS) \
	$(LIBGTOP_CFLAGS) \
	$(WARN_CFLAGS) \
	$(NULL)

mate_disk_usage_analyzer_LDFLAGS = -export-dynamic
mate_disk_usage_analyzer_LDADD = \
	-lm \
	$(GLIB_LIBS) \
	$(GIO_LIBS) \
	$(GTK_LIBS) \
	$(LIBGTOP_LIBS) \
	$(NULL)

CLEANFILES = $(BUILT_SOURCES)
