lib_LTLIBRARIES = liberis.la

# RELEASE = -release $(VERSION)-unstable

if PYTHON_BINDINGS
PYBIND_CFLAGS = -I$(top_srcdir)/boost-python -I/usr/include/python 
else
PYBIND_CFLAGS =
endif

INCLUDES += $(PYBIND_CFLAGS)
liberis_la_LDFLAGS = -version-info 0:0:0 $(RELEASE)
	
liberis_la_SOURCES = \
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
	Time.cpp \
	ArgumentDispatcher.cpp \
	TypeInfo.cpp \
	ClassDispatcher.cpp \
	InvisibleEntityCache.cpp \
	InvisibleEntityCache.h
	
liberisdir = $(includedir)/Eris

liberis_HEADERS = \
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
	Time.h \
	ArgumentDispatcher.h \
	TypeInfo.h
