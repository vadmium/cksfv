# Copyright 1999-2005 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: /build/git/uade-cvsroot/cksfv/gentoo/cksfv-1.3.3.ebuild,v 1.1 2005/02/04 18:02:20 shd Exp $

inherit eutils

DESCRIPTION="SFV checksum utility (simple file verification)"
HOMEPAGE="http://www.iki.fi/shd/foss/cksfv/"
SRC_URI="http://www.iki.fi/shd/foss/cksfv/files/${P}.tar.bz2"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="x86 ppc sparc alpha amd64"
IUSE=""

DEPEND="virtual/libc"

src_compile() {
	cd ${S}
	econf || die "configure failed"
	emake || die "compilation failed"
}

src_install() {
	dobin src/cksfv || die
	dodoc ChangeLog INSTALL README TODO
	doman cksfv.1
}
