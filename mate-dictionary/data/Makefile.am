NULL =

man_MANS = mate-dictionary.1

gdictappdir = $(datadir)/applications
gdictapp_in_files = mate-dictionary.desktop.in
gdictapp_DATA = $(gdictapp_in_files:.desktop.in=.desktop)

$(gdictapp_in_files): $(gdictapp_in_files:.desktop.in=.desktop.in.in)
	$(AM_V_GEN)sed -e "s|\@VERSION\@|@VERSION@|" $< > $@

@INTLTOOL_DESKTOP_RULE@

# dictionary sources definitions
dictsourcedir = $(datadir)/mate-dict/sources
dictsource_in_files = \
	default.desktop.in \
	thai.desktop.in \
	$(NULL)
dictsource_DATA = $(dictsource_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

builderdir = $(datadir)/mate-dictionary
builder_DATA = \
	mate-dictionary-preferences.ui	\
	mate-dictionary-source.ui	\
	$(NULL)

uidir = $(datadir)/mate-dictionary
ui_DATA = mate-dictionary-ui.xml

if BUILD_GDICT_APPLET

# applet file, dbus service, and menu definition
appletdir = $(datadir)/mate-panel/applets
applet_in_files = org.mate.DictionaryApplet.mate-panel-applet.in
applet_DATA = $(applet_in_files:.mate-panel-applet.in=.mate-panel-applet)

$(applet_in_files): $(applet_in_files).in Makefile
	$(AM_V_GEN)sed \
		-e "s|\@LIBEXECDIR\@|$(libexecdir)|" \
		$< > $@

%.mate-panel-applet: %.mate-panel-applet.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*po) ; $(INTLTOOL_MERGE) $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

servicedir = $(datadir)/dbus-1/services
service_in_files = org.mate.panel.applet.DictionaryAppletFactory.service.in
service_DATA = $(service_in_files:.service.in=.service)

org.mate.panel.applet.DictionaryAppletFactory.service: $(service_in_files)
	$(AM_V_GEN)sed \
		-e "s|\@LIBEXECDIR\@|$(libexecdir)|" \
		$< > $@

ui_DATA += dictionary-applet-menu.xml

endif # BUILD_GDICT_APPLET

@INTLTOOL_XML_RULE@
appdatadir = $(datadir)/metainfo
appdata_in_files = mate-dictionary.appdata.xml.in
appdata_DATA = $(appdata_in_files:.xml.in=.xml)

gsettings_SCHEMAS = org.mate.dictionary.gschema.xml
@GSETTINGS_RULES@

EXTRA_DIST = \
	org.mate.DictionaryApplet.mate-panel-applet.in.in \
	$(service_in_files) \
	mate-dictionary.desktop.in.in \
	$(dictsource_in_files) \
	$(man_MANS) \
	$(appdata_in_files) \
	$(builder_DATA) \
	$(ui_DATA) \
	$(NULL)

CLEANFILES = \
	$(applet_DATA) \
	$(applet_DATA).in \
	$(service_DATA) \
	$(dictsource_DATA) \
	$(gdictapp_in_files) \
	$(gdictapp_DATA) \
	$(gsettings_SCHEMAS) \
	mate-dictionary.appdata.xml \
	$(NULL)
