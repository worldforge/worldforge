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
	logStream.h \
	Avatar.cpp \
	typeService.cpp \
	view.cpp \
	router.h router.cpp \
	entityRouter.h entityRouter.cpp \
	igRouter.h igRouter.cpp \
	redispatch.h redispatch.cpp \
	factory.cpp \
	$(ATLAS_JANUS_CPP)

liberisdir = $(includedir)/Eris-1.2/Eris

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
	typeService.h \
	view.h \
	$(ATLAS_JANUS_H)

