%define name       varconf
%define version	   0.5.2
%define release    1
%define serial     1
%define prefix     /usr
%define sysconfdir /etc

Summary:           A config handling library.
Name:              %{name} 
Version:           %{version} 
Release:           %{release}
Serial:            %{serial}
Copyright:         LGPL
Group:             Amusements/Games
Source:            %{name}-%{version}.tar.gz
BuildRoot:         /var/tmp/%{name}-%{version}-buildroot
Requires:          libsigc++ >= 1.0.0 

%description
varconf is a configuration system originally designed for the STAGE server.  varconf can parse configuration files, command-line arguments and environment variables.  It supports callbacks through libsigc++ 1.0.0 or greater and can store configuration data in configuration objects or one global configuration instance.

%prep
%setup

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
%{prefix}/include/varconf/Config.h
%{prefix}/include/varconf/parse_error.h
%{prefix}/include/varconf/variable.h
%{prefix}/include/varconf/varconf.h
%{prefix}/include/varconf/dynbase.h
%{prefix}/include/varconf/dyntypes.h
%{prefix}/include/varconf/dyncmp.h
%{prefix}/include/varconf/dynvar.h
%{prefix}/lib/libvarconf.a
%{prefix}/lib/libvarconf.la
%{prefix}/lib/libvarconf.so
%{prefix}/lib/libvarconf.so.0
%{prefix}/lib/libvarconf.so.0.5.2
