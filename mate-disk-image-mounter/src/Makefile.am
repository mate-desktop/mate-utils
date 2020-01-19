NULL =

bin_PROGRAMS = mate-disk-image-mounter

mate_disk_image_mounter_SOURCES = 			\
	main.c						\
	$(NULL)

mate_disk_image_mounter_CPPFLAGS = 			\
	-I.						\
	-I$(srcdir)					\
	-DG_LOG_DOMAIN=\"mate-disk-image-mounter\"	\
	-DMATELOCALEDIR=\""$(datadir)/locale"\"		\
	$(NULL)

mate_disk_image_mounter_CFLAGS = 			\
	$(GLIB_CFLAGS)					\
	$(UDISKS2_CFLAGS)				\
	$(GTK_CFLAGS)					\
	$(WARN_CFLAGS)					\
	$(NULL)

mate_disk_image_mounter_LDADD = 			\
	$(GLIB_LIBS)					\
	$(UDISKS2_LIBS)					\
	$(GTK_LIBS)					\
	$(NULL)
