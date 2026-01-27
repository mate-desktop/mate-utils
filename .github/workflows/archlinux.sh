#!/usr/bin/bash

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
)

requires+=(
	autoconf-archive
	gcc
	git
	itstool
	libcanberra
	libgtop
	make
	mate-common
	mate-panel
	python
	udisks2
	which
	yelp-tools
	dconf
	gobject-introspection
	iso-codes
)

infobegin "Update system"
pacman --noconfirm -Syu
infoend

infobegin "Install dependency packages"
pacman --noconfirm -S ${requires[@]}
infoend
