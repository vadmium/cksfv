%define ver      1.3
%define rel      0
%define prefix   /usr

Summary: Simple File Verification program.  Does crc32 checksums on files.
Name: cksfv
Version: %ver
Release: %rel
Copyright: GPL
Group: Applications/File
Source: http://www.fodder.org/cksfv/cksfv-%{ver}.tar.gz

Patch1: cksfv-makefile.patch

URL: http://www.fodder.org/cksfv
Docdir: %{prefix}/doc

%description
cksfv (Check SFV) can create sfv (Simple File Verification) listings and test
already created sfv files.  Uses the crc32 checksum.

%prep
%setup

%patch1 -p0 -b .orig

%build
make

%install
make install

%files
%defattr(-, root, root)
%doc COPYING ChangeLog README
%prefix/bin/cksfv

%changelog
* Thu Dec 28 2000 Bryan Call  <bc@fodder.org>
- version 1.2
- Fixed the a problem when making a new sfv it was not being zero
padded (found by Justin Head and Andre Tertling).
- Added the idea, when insensitive matching is activated, it will treat
"_" and " " equal. For example it will treat "foo_bar.txt" and
"Foo Bar.txt" the same (suggested and patch given by Andree Borrmann).
- Made it so it will not choke on blank lines.

* Thu May 18 2000 Bryan Call  <bc@fodder.org>
- version 1.1
- Added verbose to the program.  Now the program will list what file it
is working on and the status of the file.  When it is finished it will
display a final result, if it encountered any errors or not, and it
will print the bell character. You can turn this off with the -q option
(quiet).
- Moved some functions around a some other minor stuff.

* Tue May 16 2000 Bryan Call  <bc@fodder.org>
- version 1.0
- This is the first version that has been released for public
consumption.  Hopefully, I have worked out most of the bugs. I have
this working on FreeBSD 3.4 and Redhat 6.1 (i386).
