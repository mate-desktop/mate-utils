#!/usr/bin/bash

set -eo pipefail

# Use grouped output messages
infobegin() {
	echo "::group::${1}"
}
infoend() {
	echo "::endgroup::"
}

# Required packages on Debian
requires=(
	ccache # Use ccache to speed up build
)

requires+=(
	autoconf-archive
	autopoint
	gcc
	g++
	git
	gtk-doc-tools
	libcanberra-gtk3-dev
	libglib2.0-dev
	libgtk-3-dev
	libgtop2-dev
	libmate-panel-applet-dev
	libudisks2-dev
	libx11-dev
	libxext-dev
	libxt-dev
	make
	mate-common
	x11proto-xext-dev
	xsltproc
	yelp-tools
	zlib1g-dev
	gobject-introspection
	iso-codes
	libdconf-dev
	libgirepository1.0-dev
)

infobegin "Update system"
apt-get update -qq
infoend

infobegin "Install dependency packages"
env DEBIAN_FRONTEND=noninteractive \
	apt-get install --assume-yes \
	${requires[@]}
infoend
