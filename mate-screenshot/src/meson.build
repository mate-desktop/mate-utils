screenshot_resources = gnome.compile_resources(
  'screenshot',
  join_paths(meson.project_source_root(), 'mate-screenshot', 'data', 'org.mate.screenshot.gresource.xml'),
  source_dir: join_paths(meson.project_source_root(), 'mate-screenshot', 'data'),
  c_name: 'screenshot',
)

mate_screenshot_sources = [
  'mate-screenshot.c',
  'screenshot-dialog.c',
  'screenshot-dialog.h',
  'screenshot-shadow.c',
  'screenshot-shadow.h',
  'screenshot-utils.c',
  'screenshot-utils.h',
  'screenshot-save.c',
  'screenshot-save.h',
  'screenshot-xfer.c',
  'screenshot-xfer.h',
]

mate_screenshot_deps = [glib_dep, gio_dep, libcanberra_gtk_dep, gthread_dep, gtk_dep]
mate_screenshot_c_args = config_c_args

if enable_wayland
  # Generate Wayland protocol client code
  protocol_xmls = [
    ['ext-image-capture-source',
      join_paths(wayland_protocols_datadir, 'staging', 'ext-image-capture-source', 'ext-image-capture-source-v1.xml')],
    ['ext-image-copy-capture',
      join_paths(wayland_protocols_datadir, 'staging', 'ext-image-copy-capture', 'ext-image-copy-capture-v1.xml')],
    ['ext-foreign-toplevel-list',
      join_paths(wayland_protocols_datadir, 'staging', 'ext-foreign-toplevel-list', 'ext-foreign-toplevel-list-v1.xml')],
    ['wlr-screencopy',
      join_paths(meson.project_source_root(), 'mate-screenshot', 'protocols', 'wlr-screencopy-unstable-v1.xml')],
  ]

  wayland_protocol_sources = []
  wayland_protocol_headers = []
  foreach proto : protocol_xmls
    name = proto[0]
    xml = proto[1]
    wayland_protocol_headers += custom_target(name + '-client-h',
      input: xml,
      output: name + '-client.h',
      command: [wayland_scanner, 'client-header', '@INPUT@', '@OUTPUT@'],
    )
    wayland_protocol_sources += custom_target(name + '-client-c',
      input: xml,
      output: name + '-client.c',
      command: [wayland_scanner, 'private-code', '@INPUT@', '@OUTPUT@'],
    )
  endforeach

  mate_screenshot_sources += [
    'wayland-screenshot.c',
    'wayland-screenshot.h',
    'wayland-select-region.c',
    'wayland-select-region.h',
  ]
  mate_screenshot_sources += wayland_protocol_sources + wayland_protocol_headers

  mate_screenshot_deps += wayland_dep
endif

executable('mate-screenshot',
  mate_screenshot_sources,
  screenshot_resources,
  include_directories: config_inc,
  dependencies: mate_screenshot_deps,
  c_args: mate_screenshot_c_args,
  link_args: ['-lm'],
  install: true,
)

install_symlink(
  'mate-panel-screenshot',
  pointing_to: 'mate-screenshot',
  install_dir: bindir,
)
