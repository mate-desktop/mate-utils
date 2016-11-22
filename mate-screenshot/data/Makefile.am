mate_screenshotdir  = $(datadir)/applications
mate_screenshot_in_files = mate-screenshot.desktop.in
mate_screenshot_DATA = $(mate_screenshot_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

man_MANS = mate-screenshot.1
uidir = $(datadir)/mate-screenshot
ui_DATA = mate-screenshot.ui

@INTLTOOL_XML_RULE@
appdatadir = $(datadir)/appdata
appdata_in_files = mate-screenshot.appdata.xml.in
appdata_DATA = $(appdata_in_files:.xml.in=.xml)

gsettings_SCHEMAS = org.mate.screenshot.gschema.xml
@GSETTINGS_RULES@

EXTRA_DIST =					\
	$(appdata_in_files)			\
	$(mate_screenshot_in_files)	\
	$(man_MANS)				\
	$(ui_DATA)

CLEANFILES = \
	$(gsettings_SCHEMAS)	\
	$(mate_screenshot_DATA)	\
	$(appdata_DATA)

dist-hook:
	cd $(distdir) ; rm -f $(CLEANFILES)

install-exec-local:
	rm -f $(DESTDIR)$(mandir)/man1/mate-panel-screenshot.1
	ln -s mate-screenshot.1 $(DESTDIR)$(mandir)/man1/mate-panel-screenshot.1

