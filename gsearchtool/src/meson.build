mate_search_tool_sources = [
  'gsearchtool-support.c',
  'gsearchtool-support.h',
  'gsearchtool-callbacks.c',
  'gsearchtool-callbacks.h',
  'gsearchtool.c',
  'gsearchtool.h',
]

executable('mate-search-tool',
  mate_search_tool_sources,
  include_directories: [
    config_inc,
    include_directories('../mate-submodules/libegg'),
    include_directories('../libmateui-deprecated'),
  ],
  dependencies: [glib_dep, gio_dep, gio_unix_dep, gtk_dep],
  link_with: [libegg, libmateui_deprecated],
  c_args: config_c_args + [
    '-DGREP_COMMAND="@0@"'.format(get_option('grep-path')),
  ],
  install: true,
)
