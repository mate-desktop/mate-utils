icondir = $(datadir)/pixmaps/mate-search-tool
icon_DATA = thumbnail_frame.png

@INTLTOOL_XML_RULE@
appdatadir = $(datadir)/appdata
appdata_in_files = mate-search-tool.appdata.xml.in
appdata_DATA = $(appdata_in_files:.xml.in=.xml)

CLEANFILES = mate-search-tool.appdata.xml

EXTRA_DIST = \
	$(icon_DATA) \
	$(appdata_in_files)
