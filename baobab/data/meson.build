schemas_conf = configuration_data()
schemas_conf.set('GETTEXT_PACKAGE', GETTEXT_PACKAGE)

configure_file(
  input: 'org.mate.disk-usage-analyzer.gschema.xml.in',
  output: 'org.mate.disk-usage-analyzer.gschema.xml',
  configuration: schemas_conf,
  install: true,
  install_dir: schemasdir,
)

desktop_conf = configuration_data()
desktop_conf.set('VERSION', meson.project_version())

desktop_in = configure_file(
  input: 'mate-disk-usage-analyzer.desktop.in.in',
  output: 'mate-disk-usage-analyzer.desktop.in',
  configuration: desktop_conf,
)

i18n.merge_file(
  input: desktop_in,
  output: 'mate-disk-usage-analyzer.desktop',
  type: 'desktop',
  po_dir: join_paths(meson.project_source_root(), 'po'),
  install: true,
  install_dir: applicationsdir,
)

i18n.merge_file(
  input: 'mate-disk-usage-analyzer.appdata.xml.in',
  output: 'mate-disk-usage-analyzer.appdata.xml',
  type: 'xml',
  po_dir: join_paths(meson.project_source_root(), 'po'),
  install: true,
  install_dir: metainfodir,
)

install_man('mate-disk-usage-analyzer.1')

# Icons
foreach size : ['16x16', '22x22', '24x24', '32x32', '48x48', '128x128']
  install_data(
    join_paths('../pixmaps', 'hicolor_apps_' + size + '_mate-disk-usage-analyzer.png'),
    install_dir: join_paths(iconsdir, size, 'apps'),
    rename: 'mate-disk-usage-analyzer.png',
  )
endforeach

install_data(
  join_paths('../pixmaps', 'hicolor_apps_scalable_mate-disk-usage-analyzer.svg'),
  install_dir: join_paths(iconsdir, 'scalable', 'apps'),
  rename: 'mate-disk-usage-analyzer.svg',
)
