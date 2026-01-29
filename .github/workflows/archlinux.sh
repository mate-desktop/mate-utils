#!/usr/bin/bash

set -eo pipefail

# Use grouped output messages
infobegin() {
	echo "::group::${1}"
}
infoend() {
	echo "::endgroup::"
}

# Required packages on Archlinux
requires=(
	ccache # Use ccache to speed up build
	clang  # Build with clang on Archlinux
)

# https://gitlab.archlinux.org/archlinux/packaging/packages/mate-utils
requires+=(
	autoconf-archive
	gcc
	gettext
	git
	glib2-devel
	inkscape
	intltool
	itstool
	libcanberra
	libgtop
	libsm
	libxml2
	make
	mate-common
	mate-desktop
	mate-panel
	python
	udisks2
	which
	yelp-tools
)

infobegin "Update system"
pacman --noconfirm -Syu
infoend

infobegin "Install dependency packages"
pacman --noconfirm -S ${requires[@]}
infoend
