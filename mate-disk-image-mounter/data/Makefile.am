NULL =

diskimagemounterdir = $(datadir)/applications
diskimagemounter_in_files = mate-disk-image-mounter.desktop.in
diskimagemounter_DATA = $(diskimagemounter_in_files:.desktop.in=.desktop)
$(diskimagemounter_DATA): $(diskimagemounter_in_files)
	$(AM_V_GEN) $(MSGFMT) --desktop --template $< -d $(top_srcdir)/po -o $@

EXTRA_DIST = 					\
	$(diskimagemounter_in_files)		\
	$(NULL)

CLEANFILES = 					\
	$(diskimagemounter_DATA)		\
	$(NULL)
