# This file is part of MATE Utils.
#
# MATE Utils is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# MATE Utils is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with MATE Utils.  If not, see <https://www.gnu.org/licenses/>.

project('mate-utils', 'c',
  version: '1.29.1',
  license: 'GPL-2.0+',
  meson_version: '>= 0.61.0',
  default_options: [
    'warning_level=1',
  ],
)

gnome = import('gnome')
i18n = import('i18n')
pkg = import('pkgconfig')

cc = meson.get_compiler('c')

prefix = get_option('prefix')
bindir = join_paths(prefix, get_option('bindir'))
datadir = join_paths(prefix, get_option('datadir'))
libdir = join_paths(prefix, get_option('libdir'))
libexecdir = join_paths(prefix, get_option('libexecdir'))
sysconfdir = join_paths(prefix, get_option('sysconfdir'))
mandir = join_paths(prefix, get_option('mandir'))
includedir = join_paths(prefix, get_option('includedir'))
pkgdatadir = join_paths(datadir, 'mate-utils')
localedir = join_paths(datadir, 'locale')
schemasdir = join_paths(datadir, 'glib-2.0', 'schemas')
applicationsdir = join_paths(datadir, 'applications')
metainfodir = join_paths(datadir, 'metainfo')
iconsdir = join_paths(datadir, 'icons', 'hicolor')

GETTEXT_PACKAGE = meson.project_name()

# Standard autoconf-style package macros (used directly by some sources)
config_h = configuration_data()
config_h.set_quoted('PACKAGE', meson.project_name())
config_h.set_quoted('PACKAGE_NAME', meson.project_name())
config_h.set_quoted('PACKAGE_VERSION', meson.project_version())
config_h.set_quoted('PACKAGE_STRING', meson.project_name() + ' ' + meson.project_version())
config_h.set_quoted('PACKAGE_TARNAME', meson.project_name())
config_h.set_quoted('VERSION', meson.project_version())

# Dependency version requirements
glib_required = '>= 2.50.0'
gio_required = '>= 2.50.0'
gio_unix_required = '>= 2.18.0'
gtk_required = '>= 3.22.0'
libmate_panel_applet_required = '>= 1.17.0'
libgtop_required = '>= 2.12.0'
libcanberra_gtk_required = '>= 0.4'
mate_desktop_required = '>= 1.27.1'
udisks2_required = '>= 1.90.0'

# Common dependencies
glib_dep = dependency('glib-2.0', version: glib_required)
gthread_dep = dependency('gthread-2.0', version: glib_required)
gio_dep = dependency('gio-2.0', version: gio_required)
gio_unix_dep = dependency('gio-unix-2.0', version: gio_unix_required)
gtk_dep = dependency('gtk+-3.0', version: gtk_required)
mate_desktop_dep = dependency('mate-desktop-2.0', version: mate_desktop_required)

# Common c_args for all components
config_h.set_quoted('GETTEXT_PACKAGE', GETTEXT_PACKAGE)
config_h.set_quoted('PREFIX', prefix)
config_h.set_quoted('SYSCONFDIR', sysconfdir)
config_h.set_quoted('LIBDIR', libdir)
config_h.set_quoted('DATADIR', datadir)
config_h.set_quoted('MATELOCALEDIR', localedir)

config_c_args = ['-include', 'config.h', '-DHAVE_CONFIG_H']

# IPv6 support
enable_ipv6 = get_option('ipv6')
if enable_ipv6
  config_h.set10('ENABLE_IPV6', true)
endif

# Wayland support
wayland_opt = get_option('wayland')
wayland_dep = []
enable_wayland = false
wayland_scanner = find_program('wayland-scanner', required: false)
wayland_protocols_dep = dependency('wayland-protocols', version: '>= 1.24', required: false)
wayland_client_dep = dependency('wayland-client', version: '>= 1.18.0', required: false)
gdk_wayland_dep = dependency('gdk-wayland-3.0', version: gtk_required, required: false)
gtk_layer_shell_dep = dependency('gtk-layer-shell-0', version: '>= 0.6', required: false)

if wayland_opt.disabled()
  enable_wayland = false
elif wayland_opt.enabled()
  if not wayland_client_dep.found() or not wayland_protocols_dep.found() or not gdk_wayland_dep.found() or not gtk_layer_shell_dep.found() or not wayland_scanner.found()
    error('Wayland support requested but not all dependencies were found:\nwayland-client >= 1.18.0, wayland-protocols >= 1.24, gdk-wayland-3.0, gtk-layer-shell-0 >= 0.6 and wayland-scanner are required')
  endif
  enable_wayland = true
else
  # auto
  if wayland_client_dep.found() and wayland_protocols_dep.found() and gdk_wayland_dep.found() and gtk_layer_shell_dep.found() and wayland_scanner.found()
    enable_wayland = true
  endif
endif

if enable_wayland
  config_h.set10('ENABLE_WAYLAND', true)
  wayland_dep = [wayland_client_dep, gdk_wayland_dep, gtk_layer_shell_dep]
  wayland_protocols_datadir = wayland_protocols_dep.get_variable(pkgconfig: 'pkgdatadir')
endif

# in-process applet
# The default follows whether Wayland is enabled, as Wayland support requires
# the in-process build (matching the autotools behavior where the default for
# --enable-in-process is the resolved --enable-wayland value).
enable_in_process = get_option('in-process') or enable_wayland
if enable_in_process
  config_h.set10('ENABLE_IN_PROCESS', true)
  config_h.set('PANEL_APPLET_FACTORY', 'MATE_PANEL_APPLET_IN_PROCESS_FACTORY')
else
  config_h.set('PANEL_APPLET_FACTORY', 'MATE_PANEL_APPLET_OUT_PROCESS_FACTORY')
endif

# Dictionary applet
build_gdict_applet = get_option('gdict-applet')
applet_dep = []
if build_gdict_applet
  applet_dep = dependency('libmatepanelapplet-4.0', version: libmate_panel_applet_required)
endif

# Disk image mounter
build_disk_image_mounter = get_option('disk-image-mounter')
udisks2_dep = []
if build_disk_image_mounter
  udisks2_dep = dependency('udisks2', version: udisks2_required)
endif

# zlib for logview
zlib_dep = dependency('zlib', required: false)
enable_zlib = get_option('zlib') and zlib_dep.found()
if enable_zlib
  config_h.set10('HAVE_ZLIB', true)
endif

# Other deps
libgtop_dep = dependency('libgtop-2.0', version: libgtop_required)
libcanberra_gtk_dep = dependency('libcanberra-gtk3', version: libcanberra_gtk_required)

# xext for screenshot
xext_dep = dependency('xext', required: false)
x11_dep = dependency('x11', required: false)
xshape_deps = [xext_dep, x11_dep]

# sm/ice for libegg
sm_dep = dependency('sm', version: '>= 1.0.0', required: false)
ice_dep = dependency('ice', version: '>= 1.0.0', required: false)

# glib tools
glib_genmarshal = find_program('glib-genmarshal')
glib_mkenums = find_program('glib-mkenums')

# itstool for help
itstool = find_program('itstool', required: get_option('help'))

enable_help = itstool.found()

# Generate config.h after all configuration_data() mutations have been made
config_file = configure_file(output: 'config.h', configuration: config_h)
config_inc = include_directories('.')

# i18n
subdir('po')

# Components
subdir('baobab')
subdir('logview')
subdir('mate-screenshot')
subdir('gsearchtool')
if build_disk_image_mounter
  subdir('mate-disk-image-mounter')
endif
subdir('mate-dictionary')

# Post-install scripts
gnome.post_install(
  glib_compile_schemas: true,
  gtk_update_icon_cache: true,
)

# Summary
summary({
  'Debug messages (libmatedict)': get_option('debug'),
  'API Reference (libmatedict)': get_option('gtk-doc'),
  'Logview ZLib support': enable_zlib,
  'Dictionary mate-panel applet': build_gdict_applet,
  'Dictionary applet in-process': enable_in_process,
  'Dictionary applet Wayland support': enable_wayland,
  'Disk Image Mounter': build_disk_image_mounter,
}, section: 'Configuration')
