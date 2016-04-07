#
# FIXME:
#  currently scrollkeeper-update does not seem to
#  remove its database when there is no useful
#  information contained there. Therefore, with
#  "make uninstall" you're left with loads of
#  stuff in /var/scrollkeeper and distcheck
#  fails.
#
#  http://mail.mate.org/archives/mate-doc-list/2003-May/msg00008.html
#
#  This overrides the uninstall check so distcheck
#  passes
#
# distuninstallcheck_listfiles = find . -type f -print | grep -v 'scrollkeeper' | grep -v 'omf' | grep -v 'figures'
distuninstallcheck:
	@:

distcleancheck_listfiles = find . -type f -print | grep -v 'omf\.out' | grep -v 'legal.xml'

ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

SUBDIRS = \
	po			\
	baobab			\
	mate-dictionary	\
	mate-screenshot	\
	gsearchtool		\
	logview			\
	$(NULL)

EXTRA_DIST = \
	autogen.sh \
	omf.make \
	xmldocs.make \
	COPYING.libs \
	COPYING.docs

DISTCLEANFILES =                \
	gtk-doc.make

DISTCHECK_CONFIGURE_FLAGS = --disable-scrollkeeper --enable-gtk-doc

# Build ChangeLog from GIT  history
ChangeLog:
	$(AM_V_GEN) if test -d $(top_srcdir)/.git; then \
		GIT_DIR="$(top_srcdir)/.git" git log --stat > $@; \
	fi

dist: ChangeLog

.PHONY: ChangeLog

