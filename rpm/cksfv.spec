# $Revision: 1.2 $, $Date: 2005/02/04 18:00:51 $
Summary:	Test archives using information from .sfv
Summary(pl):	Testowanie archiwów z u¿yciem informacji z plików .sfv
Name:		cksfv
Version:	1.3.2
Release:	1
License:	GPL
Vendor:		Bryan Call <bc@fodder.org>
Group:		Applications/Archiving
Source0:	http://www.modeemi.fi/~shd/foss/cksfv/files/%{name}-%{version}.tar.bz2
# Source0-md5:	e24b27c2f4ae36c1223b6be846ec98db
URL:		http://www.modeemi.fi/~shd/foss/cksfv/
BuildRoot:	%{tmpdir}/%{name}-%{version}-root-%(id -u -n)

%description
Utility to test .sfv files. These files are commonly used to ensure
the correct retrieval or storage of data.

%description -l pl
Narzêdzie do testowania plików .sfv. Te pliki s± czêsto u¿ywane w celu
upewnienia siê o poprawnym przesyle danych poprzez sieæ.

%prep
%setup -q

%build
./configure \
	--prefix=/usr \
	--package-prefix=$RPM_BUILD_ROOT

%{__make} \
	CFLAGS="%{rpmcflags}"

%install
rm -rf $RPM_BUILD_ROOT

%{__make} install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(644,root,root,755)
%doc ChangeLog README TODO
%attr(755,root,root) %{_bindir}/*
%{_mandir}/man1/*

%define date	%(echo `LC_ALL="C" date +"%a %b %d %Y"`)
%changelog
* %{date} PLD Team <feedback@pld-linux.org>
All persons listed below can be reached at <cvs_login>@pld-linux.org

$Log: cksfv.spec,v $
Revision 1.2  2005/02/04 18:00:51  shd
*** empty log message ***

Revision 1.17  2005/02/04 11:36:43  areq
- 1.3.2
- new URL

Revision 1.16  2004/02/05 17:59:35  qboosh
- pl fix, cosmetics

Revision 1.15  2004/02/04 23:38:49  baggins
- release 3
- merged alpha and LFS patches (it all comes to types)

Revision 1.14  2004/02/04 22:31:45  baggins
- release 2
- added large file support

Revision 1.13  2003/08/06 16:46:54  kloczek
- mo¿e wrescie kto¶ wykasuje to konto ?

Revision 1.12  2003/05/26 16:24:48  malekith
- massive attack: adding Source-md5

Revision 1.11  2003/05/25 05:46:17  misi3k
- massive attack s/pld.org.pl/pld-linux.org/

Revision 1.10  2002/10/10 12:40:25  marcus
- use new %%doc

Revision 1.9  2002/04/25 16:05:33  arturs
fixed a small typo

Revision 1.8  2002/02/23 01:33:40  kloczek
- adapterized.

Revision 1.7  2002/02/22 23:28:45  kloczek
- removed all Group fields translations (our rpm now can handle translating
  Group field using gettext).

Revision 1.6  2002/01/18 02:12:34  kloczek
perl -pi -e "s/pld-list\@pld.org.pl/feedback\@pld.org.pl/"

Revision 1.5  2001/11/02 15:54:16  baggins
- added VERSION to make

Revision 1.4  2001/11/01 20:35:36  areq
- 1.3, STBR

Revision 1.3  2001/10/12 14:21:13  baggins
- release 2
- added fixes for int/long size on alpha

Revision 1.2  2001/05/02 02:43:01  qboosh
- adapterized and made spec %%debug ready or added using %%rpm*flags macros

Revision 1.1  2001/01/07 01:02:06  misiek
new spec
