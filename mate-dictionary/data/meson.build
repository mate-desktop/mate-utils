schemas_conf = configuration_data()
schemas_conf.set('GETTEXT_PACKAGE', GETTEXT_PACKAGE)

configure_file(
  input: 'org.mate.dictionary.gschema.xml.in',
  output: 'org.mate.dictionary.gschema.xml',
  configuration: schemas_conf,
  install: true,
  install_dir: schemasdir,
)

# Main dictionary desktop file (uses .desktop.in.in -> substitute VERSION)
desktop_conf = configuration_data()
desktop_conf.set('VERSION', meson.project_version())

desktop_in = configure_file(
  input: 'mate-dictionary.desktop.in.in',
  output: 'mate-dictionary.desktop.in',
  configuration: desktop_conf,
)

i18n.merge_file(
  input: desktop_in,
  output: 'mate-dictionary.desktop',
  type: 'desktop',
  po_dir: join_paths(meson.project_source_root(), 'po'),
  install: true,
  install_dir: applicationsdir,
)

# Dictionary source definitions
foreach dictsource : ['default.desktop', 'thai.desktop']
  i18n.merge_file(
    input: dictsource + '.in',
    output: dictsource,
    type: 'desktop',
    po_dir: join_paths(meson.project_source_root(), 'po'),
    install: true,
    install_dir: join_paths(datadir, 'mate-dict', 'sources'),
  )
endforeach

# UI/XML data files
install_data('mate-dictionary-preferences.ui', 'mate-dictionary-source.ui',
  install_dir: join_paths(datadir, 'mate-dictionary'),
)
install_data('mate-dictionary-ui.xml',
  install_dir: join_paths(datadir, 'mate-dictionary'),
)

# AppData/meta info
i18n.merge_file(
  input: 'mate-dictionary.appdata.xml.in',
  output: 'mate-dictionary.appdata.xml',
  type: 'xml',
  po_dir: join_paths(meson.project_source_root(), 'po'),
  install: true,
  install_dir: metainfodir,
)

install_man('mate-dictionary.1')

if build_gdict_applet
  # Applet desktop file
  applet_conf = configuration_data()
  if enable_in_process
    applet_conf.set('APPLET_LOCATION', join_paths(prefix, libdir, meson.project_name(), 'libmate-dictionary-applet.so'))
    applet_conf.set('APPLET_IN_PROCESS', 'true')
  else
    applet_conf.set('APPLET_LOCATION', join_paths(prefix, libexecdir, 'mate-dictionary-applet'))
    applet_conf.set('APPLET_IN_PROCESS', 'false')
  endif

  applet_in = configure_file(
    input: 'org.mate.DictionaryApplet.mate-panel-applet.desktop.in.in',
    output: 'org.mate.DictionaryApplet.mate-panel-applet.desktop.in',
    configuration: applet_conf,
  )

  i18n.merge_file(
    input: applet_in,
    output: 'org.mate.DictionaryApplet.mate-panel-applet',
    type: 'desktop',
    po_dir: join_paths(meson.project_source_root(), 'po'),
    install: true,
    install_dir: join_paths(datadir, 'mate-panel', 'applets'),
  )

  install_data('dictionary-applet-menu.xml',
    install_dir: join_paths(datadir, 'mate-dictionary'),
  )

  if not enable_in_process
    # D-Bus service file for out-of-process applet
    service_conf = configuration_data()
    service_conf.set('LIBEXECDIR', libexecdir)

    service_file = configure_file(
      input: 'org.mate.panel.applet.DictionaryAppletFactory.service.in',
      output: 'org.mate.panel.applet.DictionaryAppletFactory.service',
      configuration: service_conf,
    )

    install_data(service_file,
      install_dir: join_paths(datadir, 'dbus-1', 'services'),
    )
  endif
endif
