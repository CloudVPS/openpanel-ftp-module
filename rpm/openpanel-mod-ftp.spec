%define version 0.9.3

%define libpath /usr/lib
%ifarch x86_64
  %define libpath /usr/lib64
%endif

Summary: FTP service
Name: openpanel-mod-ftp
Version: %version
Release: 1
License: GPLv2
Group: Development
Source: http://packages.openpanel.com/archive/openpanel-mod-ftp-%{version}.tar.gz
Patch1: openpanel-mod-ftp-00-makefile
BuildRoot: /var/tmp/%{name}-buildroot
Requires: openpanel-core >= 0.8.3
Requires: openpanel-mod-domain
Requires: openpanel-mod-user
Requires: openpanel-mod-apache2
Requires: pure-ftpd

%description
FTP service
Openpanel FTP management module

%prep
%setup -q -n openpanel-mod-ftp-%version
%patch1 -p0 -b .buildroot

%build
BUILD_ROOT=$RPM_BUILD_ROOT
./configure
make

%install
BUILD_ROOT=$RPM_BUILD_ROOT
rm -rf ${BUILD_ROOT}
mkdir -p ${BUILD_ROOT}/var/opencore/modules/FTP.module
cp -rf ./ftpmodule.app ${BUILD_ROOT}/var/opencore/modules/FTP.module/
ln -sf ftpmodule.app/exec ${BUILD_ROOT}/var/opencore/modules/FTP.module/action
cp module.xml ${BUILD_ROOT}/var/opencore/modules/FTP.module/
cp *.html ${BUILD_ROOT}/var/opencore/modules/FTP.module/
install -m 755 verify ${BUILD_ROOT}/var/opencore/modules/FTP.module/verify

%post
mkdir -p /var/opencore/conf/staging/FTP
chown opencore:authd /var/opencore/conf/staging/FTP
chkconfig --level 2345 pure-ftpd on
service pure-ftpd restart >/dev/null 2>&1

%files
%defattr(-,root,root)
/