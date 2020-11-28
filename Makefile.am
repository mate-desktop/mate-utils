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

distuninstallcheck:
	@:

distcleancheck_listfiles = find . -type f -print | grep -v 'omf\.out' | grep -v 'legal.xml'

ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

SUBDIRS = 			\
	po			\
	baobab			\
	mate-dictionary		\
	mate-screenshot		\
	gsearchtool		\
	logview			\
	$(NULL)

if BUILD_DISK_IMAGE_MOUNTER
SUBDIRS += 			\
	mate-disk-image-mounter
endif

EXTRA_DIST = \
	autogen.sh \
	omf.make \
	xmldocs.make \
	COPYING.libs \
	COPYING.docs \
	README.md

DISTCLEANFILES =                \
	gtk-doc.make

DISTCHECK_CONFIGURE_FLAGS = \
	--enable-gtk-doc \
	--enable-compile-warnings=no \
	CFLAGS='-Wno-deprecated-declarations'

# Build ChangeLog from GIT  history
ChangeLog:
	$(AM_V_GEN) if test -d $(top_srcdir)/.git; then \
		GIT_DIR="$(top_srcdir)/.git" git log --stat > $@; \
	fi

dist: ChangeLog

.PHONY: ChangeLog

-include $(top_srcdir)/git.mk
