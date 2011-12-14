uidir   = $(datadir)/mate-disk-usage-analyzer
ui_DATA = baobab-main-window.ui \
	baobab-dialog-scan-props.ui

baobabappdir = $(datadir)/applications
baobabapp_in_files = mate-disk-usage-analyzer.desktop.in
baobabapp_DATA = $(baobabapp_in_files:.desktop.in=.desktop)

$(baobabapp_in_files): $(baobabapp_in_files:.desktop.in=.desktop.in.in)
	sed -e "s|\@VERSION\@|@VERSION@|" $< > $@

@INTLTOOL_DESKTOP_RULE@

schemasdir = $(MATECONF_SCHEMA_FILE_DIR)
schemas_in_files = baobab.schemas.in
schemas_DATA = $(schemas_in_files:.schemas.in=.schemas)
@INTLTOOL_SCHEMAS_RULE@

man_MANS = mate-disk-usage-analyzer.1

if MATECONF_SCHEMAS_INSTALL
install-data-local:
	if test -z "$(DESTDIR)" ; then \
		MATECONF_CONFIG_SOURCE=$(MATECONF_SCHEMA_CONFIG_SOURCE) $(MATECONFTOOL) --makefile-install-rule $(schemas_DATA) ; \
	fi
else
install-data-local:
endif

EXTRA_DIST = \
	$(ui_DATA) \
	mate-disk-usage-analyzer.desktop.desktop.in.in \
	$(schemas_in_files) \
	$(man_MANS)

CLEANFILES = \
	$(baobabapp_in_files) \
	$(baobabapp_DATA) \
	$(schemas_DATA)
