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

NULL =

diskimagemounterdir = $(datadir)/applications
diskimagemounter_in_files = mate-disk-image-mounter.desktop.in
diskimagemounter_DATA = $(diskimagemounter_in_files:.desktop.in=.desktop)

$(diskimagemounter_DATA): $(diskimagemounter_in_files)
if USE_NLS
	$(AM_V_GEN) $(MSGFMT) --desktop --template $< -d $(top_srcdir)/po -o $@
else
	$(AM_V_GEN) cp $< $@
endif

EXTRA_DIST = 					\
	$(diskimagemounter_in_files)		\
	$(NULL)

CLEANFILES = 					\
	$(diskimagemounter_DATA)		\
	$(NULL)

-include $(top_srcdir)/git.mk
