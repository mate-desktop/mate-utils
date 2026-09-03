test_reader_deps = [glib_dep, gio_dep]
if enable_zlib
  test_reader_deps += zlib_dep
endif

executable('test-reader',
  sources: ['test-reader.c', '../logview-log.c', '../logview-utils.c'],
  include_directories: [include_directories('..'), config_inc],
  dependencies: test_reader_deps,
  c_args: config_c_args,
  link_args: ['-lm'],
)
