#!/usr/bin/bash

set -e
set -o pipefail

CPUS=$(grep processor /proc/cpuinfo | wc -l)

# Use grouped output messages
infobegin() {
	echo "::group::${1}"
}
infoend() {
	echo "::endgroup::"
}

if [ -f autogen.sh ]; then
	if [ "$DISTRO" = "fedora" ]; then
		# disable pt language for help in search tool
		# See: https://github.com/itstool/itstool/issues/36
		infobegin "Apply Portuguese gsearchtool help workaround"
		sed -i 's/^IGNORE_HELP_LINGUAS =.*$/IGNORE_HELP_LINGUAS = pt/' gsearchtool/help/Makefile.am
		infoend
	fi

	infobegin "Configure (autotools)"
	NOCONFIGURE=1 ./autogen.sh
	./configure --prefix=/usr --enable-compile-warnings=maximum || {
		cat config.log
		exit 1
	}
	infoend

	infobegin "Build (autotools)"
	make -j ${CPUS}
	infoend

	infobegin "Check (autotools)"
	make -j ${CPUS} check || {
		true
	}
	infoend

	infobegin "Distcheck (autotools)"
	make -j ${CPUS} distcheck
	infoend
fi
