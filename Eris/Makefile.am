INCLUDES = -I$(top_srcdir)

lib_LTLIBRARIES = liberis-1.2.la

liberis_1_2_la_LDFLAGS = -version-info @ERIS_VERSION_INFO@

if HAVE_ATLAS_JANUS
ATLAS_JANUS_CPP = UIFactory.cpp
ATLAS_JANUS_H = UIFactory.h
else
ATLAS_JANUS_CPP =
ATLAS_JANUS_H =
endif

liberis_1_2_la_SOURCES = \
	Exceptions.cpp \
	Entity.cpp \
	World.cpp \
	Lobby.cpp \
	Player.cpp \
	Dispatcher.cpp \
	BaseConnection.cpp \
	Connection.cpp \
	Utils.cpp \
	Types.cpp \
	DebugDispatcher.cpp \
	Room.cpp \
	Person.cpp \
	Metaserver.cpp \
	MetaQuery.h MetaQuery.cpp \
	ServerInfo.cpp \
	Wait.cpp \
	Timeout.cpp \
	ArgumentDispatcher.cpp \
	TypeInfo.cpp \
	ClassDispatcher.cpp \
	InvisibleEntityCache.cpp \
	InvisibleEntityCache.h \
	PollDefault.cpp \
	Log.cpp \
	Avatar.cpp \
	typeService.cpp \
	$(ATLAS_JANUS_CPP)

liberisdir = $(includedir)/Eris-1.2/Eris

liberis_HEADERS = \
	Exceptions.h \
	Lobby.h \
	Factory.h \
	Entity.h \
	World.h \
	Dispatcher.h \
	Player.h \
	Types.h \
	Utils.h \
	ClassDispatcher.h \
	TypeDispatcher.h \
	SignalDispatcher.h \
	BaseConnection.h \
	Connection.h \
	DebugDispatcher.h \
	EncapDispatcher.h \
	OpDispatcher.h \
	Room.h \
	Person.h \
	Metaserver.h \
	ServerInfo.h \
	IdDispatcher.h \
	Timeout.h \
	Timestamp.h \
	ArgumentDispatcher.h \
	TypeInfo.h \
	Wait.h \
	Property.h \
	Poll.h \
	PollDefault.h \
	atlas_utils.h \
	Log.h \
	Avatar.h \
	typeService.h \
	$(ATLAS_JANUS_H)


