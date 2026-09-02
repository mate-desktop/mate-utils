baobab_resources = gnome.compile_resources(
  'baobab',
  join_paths(meson.project_source_root(), 'baobab', 'data', 'org.mate.disk-usage-analyzer.gresource.xml'),
  source_dir: join_paths(meson.project_source_root(), 'baobab', 'data'),
  c_name: 'baobab',
)

baobab_sources = [
  'baobab.c',
  'baobab.h',
  'baobab-cell-renderer-progress.c',
  'baobab-cell-renderer-progress.h',
  'baobab-ringschart.c',
  'baobab-ringschart.h',
  'baobab-scan.c',
  'baobab-scan.h',
  'baobab-treeview.c',
  'baobab-treeview.h',
  'baobab-utils.c',
  'baobab-utils.h',
  'callbacks.c',
  'callbacks.h',
  'baobab-prefs.c',
  'baobab-prefs.h',
  'baobab-remote-connect-dialog.c',
  'baobab-remote-connect-dialog.h',
  'baobab-chart.c',
  'baobab-chart.h',
  'baobab-treemap.c',
  'baobab-treemap.h',
]

executable('mate-disk-usage-analyzer',
  baobab_sources,
  baobab_resources,
  include_directories: config_inc,
  dependencies: [glib_dep, gio_dep, gtk_dep, libgtop_dep, mate_desktop_dep],
  c_args: config_c_args + [
    '-DBAOBAB_PIX_DIR="@0@"'.format(join_paths(datadir, 'mate-disk-usage-analyzer', 'pixmaps', '')),
  ],
  link_args: ['-lm'],
  install: true,
)
