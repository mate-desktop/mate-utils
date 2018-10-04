@YELP_HELP_RULES@

HELP_ID = mate-dictionary
HELP_FILES = index.docbook legal.xml

HELP_MEDIA =                                      \
	figures/mate-dictionary-add-source.png        \
	figures/mate-dictionary-applet.png            \
	figures/mate-dictionary-applet-window.png     \
	figures/mate-dictionary-application.png       \
	figures/mate-dictionary-find.png              \
	figures/mate-dictionary-lookup.png            \
	figures/mate-dictionary-preferences-print.png \
	figures/mate-dictionary-preferences-source.png

# Add linguas to be ignored, e.g. IGNORE_HELP_LINGUAS = ca de es fr
IGNORE_HELP_LINGUAS =
HELP_LINGUAS = $(if $(IGNORE_HELP_LINGUAS), \
	$(filter-out $(IGNORE_HELP_LINGUAS),$(subst /,,$(dir $(wildcard */*.po)))), \
	$(subst /,,$(dir $(wildcard */*.po))) )
