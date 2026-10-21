schemas_conf = configuration_data()
schemas_conf.set('GETTEXT_PACKAGE', GETTEXT_PACKAGE)

configure_file(
  input: 'org.mate.search-tool.gschema.xml.in',
  output: 'org.mate.search-tool.gschema.xml',
  configuration: schemas_conf,
  install: true,
  install_dir: schemasdir,
)

i18n.merge_file(
  input: 'mate-search-tool.desktop.in',
  output: 'mate-search-tool.desktop',
  type: 'desktop',
  po_dir: join_paths(meson.project_source_root(), 'po'),
  install: true,
  install_dir: applicationsdir,
)

i18n.merge_file(
  input: 'mate-search-tool.appdata.xml.in',
  output: 'mate-search-tool.appdata.xml',
  type: 'xml',
  po_dir: join_paths(meson.project_source_root(), 'po'),
  install: true,
  install_dir: metainfodir,
)

install_man('mate-search-tool.1')

install_data('thumbnail_frame.png',
  install_dir: join_paths(datadir, 'pixmaps', 'mate-search-tool'),
)
