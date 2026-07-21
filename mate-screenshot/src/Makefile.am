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

# Wayland protocol support
if ENABLE_WAYLAND

# Protocol XML sources
EXT_IMAGE_CAPTURE_SOURCE_XML = $(WAYLAND_PROTOCOLS_DIR)/staging/ext-image-capture-source/ext-image-capture-source-v1.xml
EXT_IMAGE_COPY_CAPTURE_XML = $(WAYLAND_PROTOCOLS_DIR)/staging/ext-image-copy-capture/ext-image-copy-capture-v1.xml
EXT_FOREIGN_TOPLEVEL_XML = $(WAYLAND_PROTOCOLS_DIR)/staging/ext-foreign-toplevel-list/ext-foreign-toplevel-list-v1.xml
WLR_SCREENCOPY_XML = $(top_srcdir)/mate-screenshot/protocols/wlr-screencopy-unstable-v1.xml

# Generate per-protocol client headers and code
ext-image-capture-source-client.h: $(EXT_IMAGE_CAPTURE_SOURCE_XML)
	$(AM_V_GEN) $(WAYLAND_SCANNER) client-header $< $@

ext-image-capture-source-client.c: $(EXT_IMAGE_CAPTURE_SOURCE_XML)
	$(AM_V_GEN) $(WAYLAND_SCANNER) private-code $< $@

ext-image-copy-capture-client.h: $(EXT_IMAGE_COPY_CAPTURE_XML)
	$(AM_V_GEN) $(WAYLAND_SCANNER) client-header $< $@

ext-image-copy-capture-client.c: $(EXT_IMAGE_COPY_CAPTURE_XML)
	$(AM_V_GEN) $(WAYLAND_SCANNER) private-code $< $@

wlr-screencopy-client.h: $(WLR_SCREENCOPY_XML)
	$(AM_V_GEN) $(WAYLAND_SCANNER) client-header $< $@

wlr-screencopy-client.c: $(WLR_SCREENCOPY_XML)
	$(AM_V_GEN) $(WAYLAND_SCANNER) private-code $< $@

ext-foreign-toplevel-list-client.h: $(EXT_FOREIGN_TOPLEVEL_XML)
	$(AM_V_GEN) $(WAYLAND_SCANNER) client-header $< $@

ext-foreign-toplevel-list-client.c: $(EXT_FOREIGN_TOPLEVEL_XML)
	$(AM_V_GEN) $(WAYLAND_SCANNER) private-code $< $@

WAYLAND_PROTOCOL_SRCS =			\
	ext-image-capture-source-client.c	\
	ext-image-capture-source-client.h	\
	ext-image-copy-capture-client.c	\
	ext-image-copy-capture-client.h	\
	wlr-screencopy-client.c		\
	wlr-screencopy-client.h		\
	ext-foreign-toplevel-list-client.c	\
	ext-foreign-toplevel-list-client.h	\
	$(NULL)

# Add Wayland-specific sources and flags
mate_screenshot_SOURCES +=		\
	wayland-screenshot.c		\
	wayland-screenshot.h		\
	wayland-select-region.c		\
	wayland-select-region.h		\
	$(WAYLAND_PROTOCOL_SRCS)		\
	$(NULL)

BUILT_SOURCES +=				\
	ext-image-capture-source-client.h	\
	ext-image-copy-capture-client.h	\
	wlr-screencopy-client.h		\
	ext-foreign-toplevel-list-client.h	\
	$(NULL)

mate_screenshot_CFLAGS +=		\
	$(WAYLAND_CLIENT_CFLAGS)		\
	$(WAYLAND_PROTOCOLS_CFLAGS)	\
	$(GDK_WAYLAND_CFLAGS)		\
	$(GTK_LAYER_SHELL_CFLAGS)	\
	$(NULL)

mate_screenshot_LDADD +=		\
	$(WAYLAND_CLIENT_LIBS)		\
	$(WAYLAND_PROTOCOLS_LIBS)	\
	$(GDK_WAYLAND_LIBS)		\
	$(GTK_LAYER_SHELL_LIBS)		\
	$(NULL)
endif

CLEANFILES =				\
	$(BUILT_SOURCES)		\
	ext-image-capture-source-client.h	\
	ext-image-capture-source-client.c	\
	ext-image-copy-capture-client.h	\
	ext-image-copy-capture-client.c	\
	wlr-screencopy-client.h		\
	wlr-screencopy-client.c		\
	ext-foreign-toplevel-list-client.h	\
	ext-foreign-toplevel-list-client.c	\
	$(NULL)

install-exec-local:
	rm -f $(DESTDIR)$(bindir)/mate-panel-screenshot
	ln -s mate-screenshot $(DESTDIR)$(bindir)/mate-panel-screenshot

-include $(top_srcdir)/git.mk
