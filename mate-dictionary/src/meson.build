mate_dictionary_sources = [
  'gdict-about.c',
  'gdict-about.h',
  'gdict-app.c',
  'gdict-app.h',
  'gdict-common.c',
  'gdict-common.h',
  'gdict-pref-dialog.c',
  'gdict-pref-dialog.h',
  'gdict-print.c',
  'gdict-print.h',
  'gdict-sidebar.c',
  'gdict-sidebar.h',
  'gdict-source-dialog.c',
  'gdict-source-dialog.h',
  'gdict-window.c',
  'gdict-window.h',
  'main.c',
]

mate_dictionary_deps = [glib_dep, gio_dep, gtk_dep, mate_desktop_dep]
mate_dictionary_inc = [config_inc, include_directories('..'), include_directories('../libgdict')]
mate_dictionary_c_args = config_c_args + [
  '-DPKGDATADIR="@0@"'.format(join_paths(datadir, 'mate-dictionary')),
]

executable('mate-dictionary',
  mate_dictionary_sources,
  include_directories: mate_dictionary_inc,
  dependencies: mate_dictionary_deps,
  c_args: mate_dictionary_c_args,
  link_with: libmatedict,
  link_args: ['-lm'],
  install: true,
)

# Dictionary applet
if build_gdict_applet
  gdict_applet_sources = [
    'gdict-about.c',
    'gdict-about.h',
    'gdict-aligned-window.c',
    'gdict-aligned-window.h',
    'gdict-applet.c',
    'gdict-applet.h',
    'gdict-common.c',
    'gdict-common.h',
    'gdict-pref-dialog.c',
    'gdict-pref-dialog.h',
    'gdict-print.c',
    'gdict-print.h',
    'gdict-sidebar.c',
    'gdict-sidebar.h',
    'gdict-source-dialog.c',
    'gdict-source-dialog.h',
  ]

  gdict_applet_deps = [applet_dep, glib_dep, gio_dep, gtk_dep, mate_desktop_dep]
  gdict_applet_c_args = mate_dictionary_c_args
  if enable_wayland
    gdict_applet_deps += wayland_dep
  endif

  if enable_in_process
    pkglibdir = join_paths(libdir, meson.project_name())
    shared_library('mate-dictionary-applet',
      gdict_applet_sources,
      include_directories: mate_dictionary_inc,
      dependencies: gdict_applet_deps,
      c_args: gdict_applet_c_args,
      link_with: libmatedict,
      link_args: ['-lm'],
      install: true,
      install_dir: pkglibdir,
    )
  else
    executable('mate-dictionary-applet',
      gdict_applet_sources,
      include_directories: mate_dictionary_inc,
      dependencies: gdict_applet_deps,
      c_args: gdict_applet_c_args,
      link_with: libmatedict,
      link_args: ['-lm'],
      install: true,
      install_dir: libexecdir,
    )
  endif
endif
