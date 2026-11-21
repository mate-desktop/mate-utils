schemas_conf = configuration_data()
schemas_conf.set('GETTEXT_PACKAGE', GETTEXT_PACKAGE)

configure_file(
  input: 'org.mate.screenshot.gschema.xml.in',
  output: 'org.mate.screenshot.gschema.xml',
  configuration: schemas_conf,
  install: true,
  install_dir: schemasdir,
)

i18n.merge_file(
  input: 'mate-screenshot.desktop.in',
  output: 'mate-screenshot.desktop',
  type: 'desktop',
  po_dir: join_paths(meson.project_source_root(), 'po'),
  install: true,
  install_dir: applicationsdir,
)

i18n.merge_file(
  input: 'mate-screenshot.appdata.xml.in',
  output: 'mate-screenshot.appdata.xml',
  type: 'xml',
  po_dir: join_paths(meson.project_source_root(), 'po'),
  install: true,
  install_dir: metainfodir,
)

install_man('mate-screenshot.1')

install_symlink(
  'mate-panel-screenshot.1',
  pointing_to: 'mate-screenshot.1',
  install_dir: join_paths(mandir, 'man1'),
)
