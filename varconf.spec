%define prefix /usr

Summary: A config handling library.
Name: varconf
Version: 0.3.0
Release: 1
Copyright: GPL
Group: Amusements/Games
Source: ftp://ftp.worldforge.org/pub/WorldForge/libs/source/varconf-0.3.0.tar.gz
BuildRoot: /var/tmp/%{name}-buildroot

%description
Varconf is a configuration system designed for the STAGE server.
Varconf can pa rse configuration files, command-line arguments and
environment variables.  It s upports callbacks and can store its
configuration information in separate Config objects or in one global
configuration instance.

%prep
%setup -q

%build

if [ -x ./configure ]; then
  CFLAGS=$RPM_OPT_FLAGS ./configure --prefix=%{prefix}
else 
  CFLAGS=$RPM_OPT_FLAGS ./autogen.sh --prefix=%{prefix}
fi
make

%install
rm -rf $RPM_BUILD_ROOT
make prefix=$RPM_BUILD_ROOT%{prefix} install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{prefix}/bin/conftest
%{prefix}/include/varconf/Config.h
%{prefix}/include/varconf/Observer.h
%{prefix}/include/varconf/ParseError.h
%{prefix}/include/varconf/Variable.h
%{prefix}/include/varconf/varconf.h
%{prefix}/lib/libvarconf.a
%{prefix}/lib/libvarconf.la
%{prefix}/lib/libvarconf.so
%{prefix}/lib/libvarconf.so.0
%{prefix}/lib/libvarconf.so.0.0.0
