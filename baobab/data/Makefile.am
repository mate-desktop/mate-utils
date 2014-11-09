uidir   = $(datadir)/mate-disk-usage-analyzer
ui_DATA = baobab-main-window.ui \
	baobab-dialog-scan-props.ui

baobabappdir = $(datadir)/applications
baobabapp_in_files = mate-disk-usage-analyzer.desktop.in
baobabapp_DATA = $(baobabapp_in_files:.desktop.in=.desktop)

$(baobabapp_in_files): $(baobabapp_in_files:.desktop.in=.desktop.in.in)
	sed -e "s|\@VERSION\@|@VERSION@|" $< > $@

@INTLTOOL_DESKTOP_RULE@

gsettingsschema_in_files = org.mate.disk-usage-analyzer.gschema.xml.in
gsettings_SCHEMAS = $(gsettingsschema_in_files:.xml.in=.xml)
.PRECIOUS: $(gsettings_SCHEMAS)

@INTLTOOL_XML_RULE@
appdatadir = $(datadir)/appdata
appdata_in_files = mate-disk-usage-analyzer.appdata.xml.in
appdata_DATA = $(appdata_in_files:.xml.in=.xml)

@GSETTINGS_RULES@

convertdir = $(datadir)/MateConf/gsettings
convert_DATA = mate-disk-usage-analyzer.convert

man_MANS = mate-disk-usage-analyzer.1

EXTRA_DIST = \
	$(ui_DATA) \
	mate-disk-usage-analyzer.desktop.in.in \
	$(gsettingsschema_in_files) \
	$(convert_DATA) \
	$(appdata_in_files) \
	$(man_MANS)

CLEANFILES = \
	$(baobabapp_in_files) \
	$(baobabapp_DATA) \
	$(gsettings_SCHEMAS) \
	mate-disk-usage-analyzer.appdata.xml
