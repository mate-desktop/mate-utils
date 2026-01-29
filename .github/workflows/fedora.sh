#!/usr/bin/bash

set -eo pipefail

# Use grouped output messages
infobegin() {
	echo "::group::${1}"
}
infoend() {
	echo "::endgroup::"
}

# Required packages on Fedora
requires=(
	ccache # Use ccache to speed up build
)

requires+=(
	autoconf-archive
	desktop-file-utils
	e2fsprogs-devel
	gcc
	gcc-c++
	git
	hardlink
	libX11-devel
	libXmu-devel
	libcanberra-devel
	libgtop2-devel
	libudisks2-devel
	make
	mate-common
	mate-panel-devel
	mesa-libGL-devel
	popt-devel
	redhat-rpm-config
	usermode
	yelp-tools
	dconf-devel
	gobject-introspection-devel
	iso-codes-devel
)

infobegin "Update system"
dnf update -y
infoend

infobegin "Install dependency packages"
dnf install -y ${requires[@]}
infoend
