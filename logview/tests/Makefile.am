AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(GIO_CFLAGS) \
	-I../

noinst_PROGRAMS = test-reader

test_reader_SOURCES = test-reader.c ../logview-log.c ../logview-utils.c
test_reader_LDADD = $(GLIB_LIBS) $(GIO_LIBS) $(Z_LIBS) -lm
