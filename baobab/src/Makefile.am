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
	$(MATE_DESKTOP_CFLAGS) \
	$(WARN_CFLAGS) \
	$(NULL)

mate_disk_usage_analyzer_LDFLAGS = -export-dynamic
mate_disk_usage_analyzer_LDADD = \
	-lm \
	$(GLIB_LIBS) \
	$(GIO_LIBS) \
	$(GTK_LIBS) \
	$(LIBGTOP_LIBS) \
	$(MATE_DESKTOP_LIBS) \
	$(NULL)

CLEANFILES = $(BUILT_SOURCES)

-include $(top_srcdir)/git.mk
