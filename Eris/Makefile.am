INCLUDES = -I$(top_srcdir)

lib_LTLIBRARIES = liberis-1.3.la

liberis_1_3_la_LDFLAGS = -version-info @ERIS_VERSION_INFO@

if HAVE_ATLAS_JANUS
ATLAS_JANUS_CPP = UIFactory.cpp
ATLAS_JANUS_H = UIFactory.h
else
ATLAS_JANUS_CPP =
ATLAS_JANUS_H =
endif

liberis_1_3_la_SOURCES = \
	Exceptions.cpp \
	Entity.cpp \
	Lobby.cpp \
	Player.cpp \
	BaseConnection.cpp \
	Connection.cpp \
	Types.cpp \
	Room.cpp \
	Person.cpp \
	Metaserver.cpp \
	MetaQuery.h MetaQuery.cpp \
	ServerInfo.cpp \
	Timeout.cpp \
	TypeInfo.cpp \
	PollDefault.cpp \
	Log.cpp \
	LogStream.h \
	Avatar.cpp \
	TypeService.cpp \
	View.cpp \
	Router.h Router.cpp \
	EntityRouter.h EntityRouter.cpp \
	IGRouter.h IGRouter.cpp \
	Redispatch.h Redispatch.cpp \
	Factory.cpp \
	$(ATLAS_JANUS_CPP)

liberisdir = $(includedir)/Eris-1.3/Eris

liberis_HEADERS = \
	Exceptions.h \
	Lobby.h \
	Factory.h \
	Entity.h \
	Player.h \
	Types.h \
	Room.h \
	Person.h \
	Metaserver.h \
	ServerInfo.h \
	Timeout.h \
	TypeInfo.h \
	Poll.h \
	PollDefault.h \
	Log.h \
	Avatar.h \
	TypeService.h \
	View.h \
	$(ATLAS_JANUS_H)

