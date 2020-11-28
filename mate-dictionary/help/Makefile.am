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

@YELP_HELP_RULES@

HELP_ID = mate-dictionary
HELP_FILES = index.docbook legal.xml

HELP_MEDIA =                                      \
	figures/mate-dictionary-add-source.png        \
	figures/mate-dictionary-applet.png            \
	figures/mate-dictionary-applet-window.png     \
	figures/mate-dictionary-application.png       \
	figures/mate-dictionary-find.png              \
	figures/mate-dictionary-lookup.png            \
	figures/mate-dictionary-preferences-print.png \
	figures/mate-dictionary-preferences-source.png

# Add linguas to be ignored, e.g. IGNORE_HELP_LINGUAS = ca de es fr
IGNORE_HELP_LINGUAS =
HELP_LINGUAS = $(if $(IGNORE_HELP_LINGUAS), \
	$(filter-out $(IGNORE_HELP_LINGUAS),$(subst /,,$(dir $(wildcard */*.po)))), \
	$(subst /,,$(dir $(wildcard */*.po))) )

-include $(top_srcdir)/git.mk
