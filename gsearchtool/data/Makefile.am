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

icondir = $(datadir)/pixmaps/mate-search-tool
icon_DATA = thumbnail_frame.png

gsettingsschema_in_files = org.mate.search-tool.gschema.xml.in
gsettings_SCHEMAS = $(gsettingsschema_in_files:.xml.in=.xml)
.PRECIOUS: $(gsettings_SCHEMAS)
@GSETTINGS_RULES@

appdatadir = $(datadir)/metainfo
appdata_in_files = mate-search-tool.appdata.xml.in
appdata_DATA = $(appdata_in_files:.xml.in=.xml)
$(appdata_DATA): $(appdata_in_files)
	$(AM_V_GEN) $(MSGFMT) --xml --template $< -d $(top_srcdir)/po -o $@

man_MANS = mate-search-tool.1

Utilitiesdir = $(datadir)/applications
Utilities_in_files = mate-search-tool.desktop.in
Utilities_DATA = $(Utilities_in_files:.desktop.in=.desktop)
$(Utilities_DATA): $(Utilities_in_files)
	$(AM_V_GEN) $(MSGFMT) --desktop --template $< -d $(top_srcdir)/po -o $@

EXTRA_DIST = \
	$(icon_DATA) \
	$(appdata_in_files) \
	$(Utilities_in_files) \
	$(Utilities_DATA) \
	$(gsettingsschema_in_files) \
	$(man_MANS)

CLEANFILES = \
	mate-search-tool.appdata.xml \
	mate-search-tool.desktop \
	$(gsettings_SCHEMAS)

disthook:
	cd $(distdir) ; rm -f $(CLEANFILES)

-include $(top_srcdir)/git.mk
