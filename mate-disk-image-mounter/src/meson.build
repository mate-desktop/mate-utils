executable('mate-disk-image-mounter',
  'main.c',
  include_directories: config_inc,
  dependencies: [glib_dep, udisks2_dep, gtk_dep],
  c_args: config_c_args + [
    '-DG_LOG_DOMAIN="mate-disk-image-mounter"',
  ],
  install: true,
)
