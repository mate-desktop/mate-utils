png_icons = \
	hicolor_apps_16x16_mate-disk-usage-analyzer.png \
	hicolor_apps_22x22_mate-disk-usage-analyzer.png \
	hicolor_apps_24x24_mate-disk-usage-analyzer.png \
	hicolor_apps_32x32_mate-disk-usage-analyzer.png \
	hicolor_apps_48x48_mate-disk-usage-analyzer.png \
	hicolor_apps_128x128_mate-disk-usage-analyzer.png \
	$(NULL)

public_icons = \
	$(png_icons) \
	hicolor_apps_scalable_mate-disk-usage-analyzer.svg \
	$(NULL)

if HAVE_RSVG_CONVERT
hicolor_apps_%.png: hicolor_apps_scalable_mate-disk-usage-analyzer.svg
	$(RSVG_CONVERT) -w $(word 1, $(subst x, ,$(word 1,$(subst _, ,$*)))) -h $(word 2, $(subst x, ,$(word 1,$(subst _, ,$*)))) $^ -o $@
endif

EXTRA_DIST = \
	$(public_icons)	\
	$(NULL)

###############################################################################

install-icons: | $(public_icons)
	for icon in $(public_icons); do \
		THEME=`echo $$icon | cut -d_ -f1`; \
		CONTEXT=`echo $$icon | cut -d_ -f2`; \
		SIZE=`echo $$icon | cut -d_ -f3`; \
		ICONFILE=`echo $$icon | cut -d_ -f4`; \
		mkdir -p $(DESTDIR)$(datadir)/icons/$$THEME/$$SIZE/$$CONTEXT; \
		$(INSTALL_DATA) $(srcdir)/$$icon $(DESTDIR)$(datadir)/icons/$$THEME/$$SIZE/$$CONTEXT/$$ICONFILE; \
	done

uninstall-icons:
	-for icon in $(public_icons); do \
		THEME=`echo $$icon | cut -d_ -f1`; \
		CONTEXT=`echo $$icon | cut -d_ -f2`; \
		SIZE=`echo $$icon | cut -d_ -f3`; \
		ICONFILE=`echo $$icon | cut -d_ -f4`; \
		rm -f $(DESTDIR)$(datadir)/icons/$$THEME/$$SIZE/$$CONTEXT/$$ICONFILE; \
	done

install-data-local: install-icons

uninstall-local: uninstall-icons

gtk_update_icon_cache = gtk-update-icon-cache -f -t $(datadir)/icons/hicolor

install-data-hook: update-icon-cache
uninstall-hook: update-icon-cache
update-icon-cache:
	@-if test -z "$(DESTDIR)"; then \
		echo "Updating Gtk icon cache."; \
		$(gtk_update_icon_cache); \
	else \
		echo "*** Icon cache not updated.  After (un)install, run this:"; \
		echo "***   $(gtk_update_icon_cache)"; \
	fi

.PHONY: build-png-icons clean-png-icons
build-png-icons: | $(png_icons)

clean-png-icons:
	rm -f $(png_icons)
