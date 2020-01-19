NULL =

AM_CPPFLAGS = \
	-I$(top_srcdir)/gsearchtool/libeggsmclient \
	-I$(top_srcdir)/gsearchtool/libmateui-deprecated \
	-DMATELOCALEDIR=\""$(datadir)/locale"\" \
	-DLOCALEDIR=\"$(datadir)/locale\" \
	-DDATADIR=\""$(datadir)"\" \
	-DGREP_COMMAND=\""$(GREP_COMMAND)"\"

libeggsmclient_LIB = $(top_builddir)/gsearchtool/libeggsmclient/libeggsmclient.la
libmateui_deprecated_LIB = $(top_builddir)/gsearchtool/libmateui-deprecated/libmateui-deprecated.la

bin_PROGRAMS = mate-search-tool

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
	$(GTK_CFLAGS) \
	$(WARN_CFLAGS) \
	$(NULL)

mate_search_tool_LDADD = \
	$(GLIB_LIBS) \
	$(GIO_LIBS) \
	$(GIO_UNIX_LIBS) \
	$(GTK_LIBS) \
	$(libeggsmclient_LIB) \
	$(libmateui_deprecated_LIB)

