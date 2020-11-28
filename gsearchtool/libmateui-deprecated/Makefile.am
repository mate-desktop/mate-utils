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

noinst_LTLIBRARIES = libmateui-deprecated.la

libmateui_deprecated_la_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(GTK_CFLAGS) \
	$(WARN_CFLAGS)

libmateui_deprecated_la_LIBADD = \
	$(GLIB_LIBS) \
	$(GTK_LIBS)

libmateui_deprecated_la_SOURCES = \
	gsearchtool-entry.h	\
	gsearchtool-entry.c

-include $(top_srcdir)/git.mk
