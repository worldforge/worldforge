%define prefix /usr

Summary: The protocol library for the WorldForge project
Name: Atlas-C++
Version: 0.3.10
Release: 1
Copyright: LGPL
Group: Amusements/Games
Source: ftp://ftp.worldforge.org/pub/WorldForge/libs/source/Atlas-C++-0.3.10.tar.gz
BuildRoot: /var/tmp/%{name}-buildroot

%description
This library implements the Atlas protocol, for use in client-server
game applications.  This library is suitable for linking to either
clients or servers.

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
%{prefix}/lib/libAtlas.so.0.0.0
%{prefix}/lib/libAtlas.la
%{prefix}/lib/libAtlas.a
%{prefix}/lib/libAtlasMessage.so.0.0.0
%{prefix}/lib/libAtlasMessage.la
%{prefix}/lib/libAtlasMessage.a
%{prefix}/lib/libAtlasCodecs.so.0.0.0
%{prefix}/lib/libAtlasCodecs.la
%{prefix}/lib/libAtlasCodecs.a
%{prefix}/lib/libAtlasFilters.so.0.0.0
%{prefix}/lib/libAtlasFilters.la
%{prefix}/lib/libAtlasFilters.a
%{prefix}/lib/libAtlasNet.so.0.0.0
%{prefix}/lib/libAtlasNet.la
%{prefix}/lib/libAtlasNet.a
%{prefix}/lib/libAtlasFunky.so.0.0.0
%{prefix}/lib/libAtlasFunky.la
%{prefix}/lib/libAtlasFunky.a
%{prefix}/lib/libAtlasObjects.so.0.0.0
%{prefix}/lib/libAtlasObjects.la
%{prefix}/lib/libAtlasObjects.a
%{prefix}/lib/libAtlasObjectsEntity.so.0.0.0
%{prefix}/lib/libAtlasObjectsEntity.la
%{prefix}/lib/libAtlasObjectsEntity.a
%{prefix}/lib/libAtlasObjectsOperation.so.0.0.0
%{prefix}/lib/libAtlasObjectsOperation.la
%{prefix}/lib/libAtlasObjectsOperation.a
%{prefix}/include/Atlas/Factory.h
%{prefix}/include/Atlas/Bridge.h
%{prefix}/include/Atlas/Codec.h
%{prefix}/include/Atlas/Filter.h
%{prefix}/include/Atlas/Negotiate.h
%{prefix}/include/Atlas/Message/Object.h
%{prefix}/include/Atlas/Message/Layer.h
%{prefix}/include/Atlas/Message/Encoder.h
%{prefix}/include/Atlas/Message/DecoderBase.h
%{prefix}/include/Atlas/Message/QueuedDecoder.h
%{prefix}/include/Atlas/Net/Stream.h
%{prefix}/include/Atlas/Net/Loopback.h
%{prefix}/include/Atlas/Funky/Encoder.h
%{prefix}/include/Atlas/Task.h
%{prefix}/include/Atlas/EncoderBase.h
%{prefix}/include/Atlas/Objects/ObjectType.h
%{prefix}/include/Atlas/Objects/Root.h
%{prefix}/include/Atlas/Objects/Decoder.h
%{prefix}/include/Atlas/Objects/Encoder.h
%{prefix}/include/Atlas/Objects/Entity/Account.h
%{prefix}/include/Atlas/Objects/Entity/Admin.h
%{prefix}/include/Atlas/Objects/Entity/AdminEntity.h
%{prefix}/include/Atlas/Objects/Entity/Game.h
%{prefix}/include/Atlas/Objects/Entity/Player.h
%{prefix}/include/Atlas/Objects/Entity/RootEntity.h
%{prefix}/include/Atlas/Objects/Entity/GameEntity.h
%{prefix}/include/Atlas/Objects/Operation/Action.h
%{prefix}/include/Atlas/Objects/Operation/Appearance.h
%{prefix}/include/Atlas/Objects/Operation/Combine.h
%{prefix}/include/Atlas/Objects/Operation/Communicate.h
%{prefix}/include/Atlas/Objects/Operation/Create.h
%{prefix}/include/Atlas/Objects/Operation/Delete.h
%{prefix}/include/Atlas/Objects/Operation/Disappearance.h
%{prefix}/include/Atlas/Objects/Operation/Divide.h
%{prefix}/include/Atlas/Objects/Operation/Error.h
%{prefix}/include/Atlas/Objects/Operation/Feel.h
%{prefix}/include/Atlas/Objects/Operation/Get.h
%{prefix}/include/Atlas/Objects/Operation/Imaginary.h
%{prefix}/include/Atlas/Objects/Operation/Info.h
%{prefix}/include/Atlas/Objects/Operation/Listen.h
%{prefix}/include/Atlas/Objects/Operation/Login.h
%{prefix}/include/Atlas/Objects/Operation/Logout.h
%{prefix}/include/Atlas/Objects/Operation/Look.h
%{prefix}/include/Atlas/Objects/Operation/Move.h
%{prefix}/include/Atlas/Objects/Operation/Perceive.h
%{prefix}/include/Atlas/Objects/Operation/Perception.h
%{prefix}/include/Atlas/Objects/Operation/RootOperation.h
%{prefix}/include/Atlas/Objects/Operation/Set.h
%{prefix}/include/Atlas/Objects/Operation/Sight.h
%{prefix}/include/Atlas/Objects/Operation/Smell.h
%{prefix}/include/Atlas/Objects/Operation/Sniff.h
%{prefix}/include/Atlas/Objects/Operation/Sound.h
%{prefix}/include/Atlas/Objects/Operation/Talk.h
%{prefix}/include/Atlas/Objects/Operation/Touch.h
