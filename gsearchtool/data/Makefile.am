icondir = $(datadir)/pixmaps/mate-search-tool
icon_DATA = thumbnail_frame.png

gsettingsschema_in_files = org.mate.search-tool.gschema.xml.in
gsettings_SCHEMAS = $(gsettingsschema_in_files:.xml.in=.xml)
.PRECIOUS: $(gsettings_SCHEMAS)

@INTLTOOL_DESKTOP_RULE@

@GSETTINGS_RULES@

@INTLTOOL_XML_RULE@
appdatadir = $(datadir)/metainfo
appdata_in_files = mate-search-tool.appdata.xml.in
appdata_DATA = $(appdata_in_files:.xml.in=.xml)

man_MANS = mate-search-tool.1

Utilitiesdir = $(datadir)/applications
Utilities_in_files = mate-search-tool.desktop.in
Utilities_DATA = $(Utilities_in_files:.desktop.in=.desktop)

EXTRA_DIST = \
	$(icon_DATA) \
	$(appdata_in_files) \
	$(Utilities_in_files) \
	$(Utilities_DATA) \
	$(gsettingsschema_in_files) \
	$(man_MANS)

CLEANFILES = \
	mate-search-tool.appdata.xml \
	mate-search-tool.desktop \
	$(gsettings_SCHEMAS)

disthook:
	cd $(distdir) ; rm -f $(CLEANFILES)
