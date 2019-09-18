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
	COPYING.docs

DISTCLEANFILES =                \
	gtk-doc.make

DISTCHECK_CONFIGURE_FLAGS = \
	--enable-gtk-doc \
	CFLAGS='-Wno-deprecated-declarations'

# Build ChangeLog from GIT  history
ChangeLog:
	$(AM_V_GEN) if test -d $(top_srcdir)/.git; then \
		GIT_DIR="$(top_srcdir)/.git" git log --stat > $@; \
	fi

dist: ChangeLog

.PHONY: ChangeLog

