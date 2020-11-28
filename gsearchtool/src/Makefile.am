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
	-I$(top_srcdir)/gsearchtool/mate-submodules/libegg \
	-I$(top_srcdir)/gsearchtool/libmateui-deprecated \
	-DMATELOCALEDIR=\""$(datadir)/locale"\" \
	-DLOCALEDIR=\"$(datadir)/locale\" \
	-DDATADIR=\""$(datadir)"\" \
	-DGREP_COMMAND=\""$(GREP_COMMAND)"\"

libeggsmclient_LIB = $(top_builddir)/gsearchtool/mate-submodules/libegg/libegg.la
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

-include $(top_srcdir)/git.mk
