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

AM_CPPFLAGS = -DG_LOG_DOMAIN=\"Gdict\" 				\
	-DDATADIR=\""$(datadir)"\" 				\
	-DLIBDIR=\""$(libdir)"\" 				\
	-DSYSCONFDIR=\""$(sysconfdir)"\" 			\
	-DPREFIX=\""$(prefix)"\" 				\
	-DMATELOCALEDIR=\""$(datadir)/locale"\"			\
	-DGDICTSOURCESDIR=\""$(datadir)/mate-dict/sources"\" 	\
	-DGDICT_ENABLE_INTERNALS				\
	$(NULL)

sources_h =	\
	$(srcdir)/gdict-context.h		\
	$(srcdir)/gdict-client-context.h	\
	$(srcdir)/gdict-database-chooser.h 	\
	$(srcdir)/gdict-defbox.h 		\
	$(srcdir)/gdict-source-chooser.h	\
	$(srcdir)/gdict-source-loader.h		\
	$(srcdir)/gdict-source.h		\
	$(srcdir)/gdict-speller.h		\
	$(srcdir)/gdict-strategy-chooser.h 	\
	$(srcdir)/gdict-utils.h			\
	$(NULL)

sources_h_priv =	\
	$(srcdir)/gdict-context-private.h	\
	$(srcdir)/gdict-debug.h			\
	$(srcdir)/gdict-private.h		\
	$(NULL)

sources_c =	\
	$(srcdir)/gdict-context.c		\
	$(srcdir)/gdict-client-context.c	\
	$(srcdir)/gdict-database-chooser.c	\
	$(srcdir)/gdict-defbox.c		\
	gdict-enum-types.c			\
	gdict-marshal.c				\
	$(srcdir)/gdict-source-chooser.c	\
	$(srcdir)/gdict-source-loader.c		\
	$(srcdir)/gdict-source.c		\
	$(srcdir)/gdict-speller.c		\
	$(srcdir)/gdict-strategy-chooser.c	\
	$(srcdir)/gdict-utils.c			\
	$(NULL)

lib_LTLIBRARIES = libmatedict.la

# MATE Dictionary shared library
libmatedict_la_SOURCES  = $(sources_h) $(sources_h_priv) $(sources_c)
libmatedict_la_CPPFLAGS = 	\
	$(AM_CPPFLAGS)		\
	$(LIBGDICT_CFLAGS)	\
	$(GDICT_DEBUG_CFLAGS)	\
	$(WARN_CFLAGS)		\
	$(NULL)
libmatedict_la_LIBADD   = $(LIBGDICT_LIBS)
libmatedict_la_LDFLAGS  = \
	-version-info $(LIBGDICT_LT_VERSION) \
	-export-dynamic \
	-no-undefined

libgdict_includedir = $(includedir)/mate-dict/gdict
libgdict_include_HEADERS = 	\
	$(sources_h)		\
	gdict-enum-types.h	\
	gdict-version.h		\
	$(srcdir)/gdict.h	\
	$(NULL)

gdict_built_files = \
	gdict-enum-types.h \
	gdict-enum-types.c \
	gdict-marshal.h \
	gdict-marshal.c

stamp_files = stamp-gdict-enum-types.h stamp-gdict-marshal.h

CLEANFILES = $(stamp_files) $(gdict_built_files)
DISTCLEANFILES = gdict-version.h
MAINTAINERCLEANFILES = $(stamp_files) $(gdict_built_files) gdict-version.h

EXTRA_DIST = \
	gdict-version.h.in \
	gdict-enum-types.h.in \
	gdict-enum-types.c.in \
	gdict-marshal.list \
	mate-dict.pc.in \
	$(NULL)

BUILT_SOURCES = $(gdict_built_files)

gdict-marshal.h: stamp-gdict-marshal.h
	@true
stamp-gdict-marshal.h: gdict-marshal.list $(GLIB_GENMARSHAL) Makefile
	$(AM_V_GEN)$(GLIB_GENMARSHAL) $< --header --prefix=gdict_marshal > xgen-gmh \
	&& ( cmp -s xgen-gmh gdict-marshal.h || cp xgen-gmh gdict-marshal.h ) \
	&& rm -f xgen-gmh \
	&& echo timestamp > $@

gdict-marshal.c: gdict-marshal.list $(GLIB_GENMARSHAL) Makefile
	$(AM_V_GEN)( echo "#include \"gdict-marshal.h\""; echo; \
	  $(GLIB_GENMARSHAL) $< --body --prefix=gdict_marshal ) > xgen-gmc \
	&& cp xgen-gmc gdict-marshal.c \
	&& rm -f xgen-gmc

gdict-enum-types.h: stamp-gdict-enum-types.h
	@true
stamp-gdict-enum-types.h: $(sources_h) gdict-enum-types.h.in Makefile
	$(AM_V_GEN) $(GLIB_MKENUMS) --template $(srcdir)/gdict-enum-types.h.in \
	  $(sources_h) > xgen-ceth \
	&& ( cmp -s xgen-ceth gdict-enum-types.h || cp xgen-ceth gdict-enum-types.h ) \
	&& rm -f xgen-ceth \
	&& echo timestamp > $@

gdict-enum-types.c: gdict-enum-types.h gdict-enum-types.c.in Makefile
	$(AM_V_GEN) $(GLIB_MKENUMS) --template $(srcdir)/gdict-enum-types.c.in \
	  $(sources_h) > xgen-cetc \
	&& cp xgen-cetc gdict-enum-types.c \
	&& rm -f xgen-cetc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = mate-dict.pc

distclean-local:
	if test $(srcdir) != .; then \
		rm -f $(MAINTAINERCLEANFILES); \
	fi

-include $(top_srcdir)/git.mk
