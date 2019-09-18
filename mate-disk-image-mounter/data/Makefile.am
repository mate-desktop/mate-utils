NULL =

diskimagemounterdir = $(datadir)/applications
diskimagemounter_in_files = mate-disk-image-mounter.desktop.in
diskimagemounter_DATA = $(diskimagemounter_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST = 					\
	$(diskimagemounter_in_files)		\
	$(NULL)

CLEANFILES = 					\
	$(diskimagemounter_DATA)		\
	$(NULL)
