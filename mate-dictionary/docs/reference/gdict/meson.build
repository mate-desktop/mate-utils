gdict_version_xml = configure_file(
  input: 'version.xml.in',
  output: 'version.xml',
  configuration: {'GDICT_VERSION': '1.1.0'},
)

gnome.gtkdoc(
  'mate-dict',
  main_sgml: 'gdict-docs.sgml',
  src_dir: join_paths(meson.project_source_root(), 'mate-dictionary', 'libgdict'),
  gobject_typesfile: 'gdict.types',
  ignore_headers: [
    'gdict-enum-types.h',
    'gdict-marshal.h',
    'gdict-context-private.h',
    'gdict-debug.h',
    'gdict-private.h',
    'stamp-gdict-enum-types.h',
    'stamp-gdict-marshal.h',
  ],
  content_files: gdict_version_xml,
  mkdb_args: ['--sgml-mode', '--output-format=xml'],
  dependencies: [glib_dep, gtk_dep, libmatedict_dep],
  install: true,
)
