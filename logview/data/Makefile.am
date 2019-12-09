SUBDIRS=icons

desktopdir = $(datadir)/applications
desktop_in_files = mate-system-log.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
$(desktop_in_files): $(desktop_in_files:.desktop.in=.desktop.in.in)
	@sed -e "s|\@VERSION\@|@VERSION@|" $< > $@
@INTLTOOL_DESKTOP_RULE@

man_MANS = mate-system-log.1

gsettings_SCHEMAS = org.mate.system-log.gschema.xml

@INTLTOOL_XML_RULE@

@GSETTINGS_RULES@

EXTRA_DIST = 			\
	logview-filter.ui	\
	logview-toolbar.xml	\
	mate-system-log.desktop.in.in	\
	$(man_MANS)		\
	org.mate.system-log.gresource.xml

DISTCLEANFILES =		\
	$(desktop_DATA)		\
	$(desktop_in_files)	\
	$(gsettings_SCHEMAS)
