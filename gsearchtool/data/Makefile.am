icondir = $(datadir)/pixmaps/mate-search-tool
icon_DATA = thumbnail_frame.png

gsettingsschema_in_files = org.mate.search-tool.gschema.xml.in
gsettings_SCHEMAS = $(gsettingsschema_in_files:.xml.in=.xml)
.PRECIOUS: $(gsettings_SCHEMAS)
@GSETTINGS_RULES@

appdatadir = $(datadir)/metainfo
appdata_in_files = mate-search-tool.appdata.xml.in
appdata_DATA = $(appdata_in_files:.xml.in=.xml)
$(appdata_DATA): $(appdata_in_files)
	$(AM_V_GEN) $(MSGFMT) --xml --template $< -d $(top_srcdir)/po -o $@

man_MANS = mate-search-tool.1

Utilitiesdir = $(datadir)/applications
Utilities_in_files = mate-search-tool.desktop.in
Utilities_DATA = $(Utilities_in_files:.desktop.in=.desktop)
$(Utilities_DATA): $(Utilities_in_files)
	$(AM_V_GEN) $(MSGFMT) --desktop --template $< -d $(top_srcdir)/po -o $@

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
