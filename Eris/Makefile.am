lib_LTLIBRARIES = liberis.la


if PYTHON_BINDINGS
PYBIND_CFLAGS = -I$(top_srcdir)/boost-python -I/usr/include/python 
else
PYBIND_CFLAGS =
endif

## Can't do this while Types.cpp needs the conversion stuff in Types.h
##AM_CXXFLAGS = -DWFMATH_CONVERSION

INCLUDES += $(PYBIND_CFLAGS)
liberis_la_LDFLAGS = -version-info @ERIS_VERSION_INFO@
	
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
	Timestamp.cpp \
	ArgumentDispatcher.cpp \
	TypeInfo.cpp \
	ClassDispatcher.cpp \
	InvisibleEntityCache.cpp \
	InvisibleEntityCache.h \
	PollDefault.cpp \
	Log.cpp
	
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
	Timestamp.h \
	ArgumentDispatcher.h \
	TypeInfo.h \
	Wait.h \
	Property.h \
	Poll.h \
	PollDefault.h \
	atlas_utils.h \
	Log.h
	
	
