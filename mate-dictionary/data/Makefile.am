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
	spanish.desktop.in \
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

# applet menu definition
appletuidir = $(datadir)/mate-2.0/ui
appletui_DATA = MATE_DictionaryApplet.xml

serverdir = $(libdir)/matecomponent/servers
server_in_files = MATE_DictionaryApplet.server.in
server_DATA = $(server_in_files:.server.in=.server)

$(server_in_files): $(server_in_files:.server.in=.server.in.in)
	$(AM_V_GEN)sed 					\
		-e "s|\@VERSION\@|@VERSION@|" 		\
		-e "s|\@LIBEXECDIR\@|$(libexecdir)|" 	\
	$< > $@

@INTLTOOL_SERVER_RULE@

endif # BUILD_GDICT_APPLET

gsettingsschema_in_files = org.mate.dictionary.gschema.xml.in
gsettings_SCHEMAS = $(gsettingsschema_in_files:.xml.in=.xml)
@INTLTOOL_XML_NOMERGE_RULE@
@GSETTINGS_RULES@

convertdir = $(datadir)/MateConf/gsettings
convert_DATA = mate-dictionary.convert

EXTRA_DIST = \
	MATE_DictionaryApplet.server.in.in \
	mate-dictionary.desktop.in.in \
	$(dictsource_in_files) \
	$(man_MANS) \
	$(gsettingsschema_in_files) \
	$(builder_DATA) \
	$(ui_DATA) \
	$(appletui_DATA) \
	$(NULL)

CLEANFILES = \
	$(dictsource_DATA) \
	$(server_in_files) \
	$(server_DATA) \
	$(gdictapp_in_files) \
	$(gdictapp_DATA) \
	$(gsettings_SCHEMAS) \
	$(NULL)
