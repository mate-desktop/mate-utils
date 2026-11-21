logview_marshal = gnome.genmarshal(
  'logview-marshal',
  sources: 'logview-marshal.list',
  prefix: 'logview_marshal',
)

logview_resources = gnome.compile_resources(
  'logview',
  join_paths(meson.project_source_root(), 'logview', 'data', 'org.mate.system-log.gresource.xml'),
  source_dir: join_paths(meson.project_source_root(), 'logview', 'data'),
  c_name: 'logview',
)

logview_sources = [
  'logview-app.c',
  'logview-app.h',
  'logview-main.c',
  'logview-about.h',
  'logview-manager.c',
  'logview-manager.h',
  'logview-utils.c',
  'logview-utils.h',
  'logview-loglist.c',
  'logview-loglist.h',
  'logview-window.c',
  'logview-window.h',
  'logview-log.c',
  'logview-log.h',
  'logview-findbar.h',
  'logview-findbar.c',
  'logview-prefs.c',
  'logview-prefs.h',
  'logview-filter.h',
  'logview-filter.c',
  'logview-filter-manager.h',
  'logview-filter-manager.c',
]

logview_deps = [glib_dep, gthread_dep, gio_dep, gtk_dep]
if enable_zlib
  logview_deps += zlib_dep
endif

logview_c_args = config_c_args + [
  '-DG_LOG_DOMAIN="mate-system-log"',
  '-DLOGVIEWINSTALLPREFIX="@0@/"'.format(prefix),
]

executable('mate-system-log',
  logview_sources,
  logview_marshal,
  logview_resources,
  include_directories: config_inc,
  dependencies: logview_deps,
  c_args: logview_c_args,
  link_args: ['-lm'],
  install: true,
)

subdir('tests')
