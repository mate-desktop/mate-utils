gdict_sources_h = [
  'gdict-context.h',
  'gdict-client-context.h',
  'gdict-database-chooser.h',
  'gdict-defbox.h',
  'gdict-source-chooser.h',
  'gdict-source-loader.h',
  'gdict-source.h',
  'gdict-speller.h',
  'gdict-strategy-chooser.h',
  'gdict-utils.h',
]

gdict_sources_h_priv = [
  'gdict-context-private.h',
  'gdict-debug.h',
  'gdict-private.h',
]

gdict_sources_c = [
  'gdict-context.c',
  'gdict-client-context.c',
  'gdict-database-chooser.c',
  'gdict-defbox.c',
  'gdict-source-chooser.c',
  'gdict-source-loader.c',
  'gdict-source.c',
  'gdict-speller.c',
  'gdict-strategy-chooser.c',
  'gdict-utils.c',
]

# gdict-version.h (substitute GDICT_* version macros)
gdict_version_conf = configuration_data()
gdict_version_conf.set('GDICT_MAJOR_VERSION', 1)
gdict_version_conf.set('GDICT_MINOR_VERSION', 1)
gdict_version_conf.set('GDICT_MICRO_VERSION', 0)
gdict_version_conf.set('GDICT_VERSION', '1.1.0')

gdict_version_h = configure_file(
  input: 'gdict-version.h.in',
  output: 'gdict-version.h',
  configuration: gdict_version_conf,
  install: true,
  install_dir: join_paths(includedir, 'mate-dict', 'gdict'),
)

# gdict-enum-types.h and .c using the custom templates
gdict_enum_types_h = custom_target('gdict-enum-types.h',
  input: gdict_sources_h,
  output: 'gdict-enum-types.h',
  command: [glib_mkenums,
    '--template', '@0@/gdict-enum-types.h.in'.format(meson.current_source_dir()),
    '@INPUT@',
  ],
  capture: true,
  install: true,
  install_dir: join_paths(includedir, 'mate-dict', 'gdict'),
)

gdict_enum_types_c = custom_target('gdict-enum-types.c',
  input: gdict_sources_h,
  output: 'gdict-enum-types.c',
  command: [glib_mkenums,
    '--template', '@0@/gdict-enum-types.c.in'.format(meson.current_source_dir()),
    '@INPUT@',
  ],
  capture: true,
)

# gdict-marshal.h and .c
gdict_marshal_h = custom_target('gdict-marshal.h',
  input: 'gdict-marshal.list',
  output: 'gdict-marshal.h',
  command: [glib_genmarshal, '@INPUT@', '--header', '--prefix=gdict_marshal'],
  capture: true,
)

gdict_marshal_c = custom_target('gdict-marshal.c',
  input: 'gdict-marshal.list',
  output: 'gdict-marshal.c',
  command: [glib_genmarshal, '@INPUT@', '--body', '--prefix=gdict_marshal'],
  capture: true,
)

libgdict_c_args = config_c_args + [
  '-DG_LOG_DOMAIN="Gdict"',
  '-DGDICTSOURCESDIR="@0@"'.format(join_paths(datadir, 'mate-dict', 'sources')),
  '-DGDICT_ENABLE_INTERNALS',
  '-DGDICT_ENABLE_DEBUG',
]

libmatedict = shared_library('matedict',
  gdict_sources_h,
  gdict_sources_h_priv,
  gdict_sources_c,
  gdict_enum_types_h,
  gdict_enum_types_c,
  gdict_marshal_h,
  gdict_marshal_c,
  gdict_version_h,
  include_directories: config_inc,
  dependencies: [glib_dep, gtk_dep],
  c_args: libgdict_c_args,
  version: '6.7.0',
  install: true,
  install_dir: libdir,
)

libmatedict_dep = declare_dependency(
  link_with: libmatedict,
  include_directories: include_directories('.'),
)

install_headers(
  gdict_sources_h + ['gdict.h'],
  subdir: 'mate-dict/gdict',
)

pkg.generate(libmatedict,
  filebase: 'mate-dict',
  name: 'gdict-1.0',
  description: 'MATE Dictionary Protocol client library',
  version: '1.1.0',
  requires: [gtk_dep],
  subdirs: 'mate-dict',
  install_dir: join_paths(libdir, 'pkgconfig'),
)
