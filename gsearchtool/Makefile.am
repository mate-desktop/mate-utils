Utilitiesdir = $(datadir)/applications

SUBDIRS = data help libmateui-deprecated

AM_CPPFLAGS = \
	-I$(srcdir)/libmateui-deprecated \
	-I$(top_srcdir)/libeggsmclient \
	-DG_DISABLE_DEPRECATED \
	-DMATELOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DLOCALEDIR=\"$(datadir)/locale\" \
	-DDATADIR=\""$(datadir)"\" \
	-DGREP_COMMAND=\""$(GREP_COMMAND)"\"

libmateui_deprecated_LIB = $(builddir)/libmateui-deprecated/libmateui-deprecated.la
libeggsmclient_LIB = $(top_builddir)/libeggsmclient/libeggsmclient.la

bin_PROGRAMS = mate-search-tool

gsettingsschema_in_files = org.mate.search-tool.gschema.xml.in
gsettings_SCHEMAS = $(gsettingsschema_in_files:.xml.in=.xml)
.PRECIOUS: $(gsettings_SCHEMAS)

@INTLTOOL_XML_NOMERGE_RULE@

@GSETTINGS_RULES@

convertdir = $(datadir)/MateConf/gsettings
convert_DATA = mate-search-tool.convert

mate_search_tool_SOURCES = \
	gsearchtool-support.c \
	gsearchtool-support.h \
	gsearchtool-callbacks.c \
	gsearchtool-callbacks.h \
	gsearchtool.c \
	gsearchtool.h

mate_search_tool_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(GIO_CFLAGS) \
	$(GIO_UNIX_CFLAGS) \
	$(GTK_CFLAGS)

mate_search_tool_LDADD = \
	$(GLIB_LIBS) \
	$(GIO_LIBS) \
	$(GIO_UNIX_LIBS) \
	$(GTK_LIBS) \
	$(libmateui_deprecated_LIB) \
	$(libeggsmclient_LIB)

man_MANS = mate-search-tool.1

Utilities_in_files = mate-search-tool.desktop.in
Utilities_DATA = $(Utilities_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST = \
	$(Utilities_in_files) \
	$(Utilities_DATA) \
	$(gsettingsschema_in_files) \
	$(man_MANS)

CLEANFILES = \
	mate-search-tool.desktop \
	$(convert_DATA) \
	$(gsettings_SCHEMAS)

disthook:
	cd $(distdir) ; rm -f $(CLEANFILES)
