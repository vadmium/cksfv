%define ver      1.1
%define rel      0
%define prefix   /usr

Summary: Simple File Verification program.  Does crc32 checksums on files.
Name: cksfv
Version: %ver
Release: %rel
Copyright: GPL
Group: Applications/File
Source: http://www.fodder.org/cksfv/cksfv-%{ver}.tar.gz

Patch1: cksfv-1.0-makefile.patch

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
