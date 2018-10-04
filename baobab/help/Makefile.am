@YELP_HELP_RULES@

HELP_ID = mate-disk-usage-analyzer
HELP_FILES = index.docbook legal.xml

HELP_MEDIA =                       \
	figures/baobab_fullscan.png    \
	figures/baobab_prefs.png       \
	figures/baobab_remote.png      \
	figures/baobab_ringschart1.png \
	figures/baobab_ringschart2.png \
	figures/baobab_treemaps.png    \
	figures/baobab_window.png

# Add linguas to be ignored, e.g. IGNORE_HELP_LINGUAS = ca de es fr
IGNORE_HELP_LINGUAS =
HELP_LINGUAS = $(if $(IGNORE_HELP_LINGUAS), \
	$(filter-out $(IGNORE_HELP_LINGUAS),$(subst /,,$(dir $(wildcard */*.po)))), \
	$(subst /,,$(dir $(wildcard */*.po))) )
