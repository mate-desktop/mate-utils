baobabappdir = $(datadir)/applications
baobabapp_in_files = mate-disk-usage-analyzer.desktop.in
baobabapp_DATA = $(baobabapp_in_files:.desktop.in=.desktop)

$(baobabapp_in_files): $(baobabapp_in_files:.desktop.in=.desktop.in.in)
	sed -e "s|\@VERSION\@|@VERSION@|" $< > $@

$(baobabapp_DATA): $(baobabapp_in_files)
	$(AM_V_GEN) $(MSGFMT) --desktop --template $< -d $(top_srcdir)/po -o $@

appdatadir = $(datadir)/metainfo
appdata_in_files = mate-disk-usage-analyzer.appdata.xml.in
appdata_DATA = $(appdata_in_files:.xml.in=.xml)
$(appdata_DATA): $(appdata_in_files)
	$(AM_V_GEN) $(MSGFMT) --xml --template $< -d $(top_srcdir)/po -o $@

gsettings_SCHEMAS = org.mate.disk-usage-analyzer.gschema.xml
@GSETTINGS_RULES@

man_MANS = mate-disk-usage-analyzer.1

EXTRA_DIST = \
	baobab-main-window.ui \
	baobab-dialog-scan-props.ui \
	mate-disk-usage-analyzer.desktop.in.in \
	$(appdata_in_files) \
	$(man_MANS) \
	org.mate.disk-usage-analyzer.gresource.xml

CLEANFILES = \
	$(baobabapp_in_files) \
	$(baobabapp_DATA) \
	$(gsettings_SCHEMAS) \
	$(appdata_DATA)
