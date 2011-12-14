NULL =

INCLUDES = \
	-DPREFIX=\""$(prefix)"\" \
	-DSYSCONFDIR=\""$(sysconfdir)"\" \
	-DLIBDIR=\""$(libdir)"\" \
	-DDATADIR=\""$(datadir)"\" \
	-DPKGDATADIR=\""$(datadir)/mate-disk-usage-analyzer"\" \
	-DBAOBAB_PIX_DIR=\""$(datadir)/mate-disk-usage-analyzer/pixmaps/"\" \
	-DMATELOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	$(NULL)

bin_PROGRAMS = mate-disk-usage-analyzer

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
	$(MATE_UTILS_CFLAGS) \
	$(LIBGTOP_CFLAGS) \
	$(NULL)

mate_disk_usage_analyzer_LDFLAGS = -export-dynamic
mate_disk_usage_analyzer_LDADD = \
	-lm \
	$(MATE_UTILS_LIBS) \
	$(LIBGTOP_LIBS) \
	$(NULL)
