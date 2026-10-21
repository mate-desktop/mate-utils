schemas_conf = configuration_data()
schemas_conf.set('GETTEXT_PACKAGE', GETTEXT_PACKAGE)

configure_file(
  input: 'org.mate.system-log.gschema.xml.in',
  output: 'org.mate.system-log.gschema.xml',
  configuration: schemas_conf,
  install: true,
  install_dir: schemasdir,
)

desktop_conf = configuration_data()
desktop_conf.set('VERSION', meson.project_version())

desktop_in = configure_file(
  input: 'mate-system-log.desktop.in.in',
  output: 'mate-system-log.desktop.in',
  configuration: desktop_conf,
)

i18n.merge_file(
  input: desktop_in,
  output: 'mate-system-log.desktop',
  type: 'desktop',
  po_dir: join_paths(meson.project_source_root(), 'po'),
  install: true,
  install_dir: applicationsdir,
)

install_man('mate-system-log.1')

# Icons
foreach size : ['16x16', '22x22', '24x24', '32x32', '48x48', '256x256']
  install_data(
    join_paths('icons', 'hicolor_apps_' + size + '_mate-system-log.png'),
    install_dir: join_paths(iconsdir, size, 'apps'),
    rename: 'mate-system-log.png',
  )
endforeach

install_data(
  join_paths('icons', 'hicolor_apps_scalable_mate-system-log-symbolic.svg'),
  install_dir: join_paths(iconsdir, 'scalable', 'apps'),
  rename: 'mate-system-log-symbolic.svg',
)
