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

bin_PROGRAMS = mate-disk-image-mounter

mate_disk_image_mounter_SOURCES = 			\
	main.c						\
	$(NULL)

mate_disk_image_mounter_CPPFLAGS = 			\
	-I.						\
	-I$(srcdir)					\
	-DG_LOG_DOMAIN=\"mate-disk-image-mounter\"	\
	-DMATELOCALEDIR=\""$(datadir)/locale"\"		\
	$(NULL)

mate_disk_image_mounter_CFLAGS = 			\
	$(GLIB_CFLAGS)					\
	$(UDISKS2_CFLAGS)				\
	$(GTK_CFLAGS)					\
	$(WARN_CFLAGS)					\
	$(NULL)

mate_disk_image_mounter_LDADD = 			\
	$(GLIB_LIBS)					\
	$(UDISKS2_LIBS)					\
	$(GTK_LIBS)					\
	$(NULL)

-include $(top_srcdir)/git.mk
