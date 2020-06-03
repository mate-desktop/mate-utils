mate-utils
=========================
Contains MATE Utility programs for the MATE Desktop, namely the following:

* mate-system-log          [logview]
* mate-search-tool         [gsearchtool]
* mate-dictionary          [mate-dictionary]
* mate-screenshot          [mate-screenshot]
* mate-disk-usage-analyzer [baobab]

mate-utils is a fork of GNOME Utilities. 
This software is licensed under the GNU GPL. For more on the license, see COPYING.

Requirements
========================
* intltool                 >= 0.50.1
* mate-common              >= 1.24.1
* GLib                     >= 2.50.0
* GIO                      >= 2.50.0
* GTK+                     >= 3.22.0
* libmate-panel-applet     >= 1.17.0
* libgtop                  >= 2.12.0
* libcanberra-gtk          >= 0.4

The following configure flags can be used:

  `--enable-zlib`: Enable ZLib support for Logview [default=yes]
  `--with-grep`: Specify the path to the grep command [default=find it ourselves]
  `--enable-debug`: Enable debug messages [default=no]
