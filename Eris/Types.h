#ifndef ERIS_TYPES_H
#define ERIS_TYPES_H

// system headers
#include <vector>
#include <string>
#include <list>
#include <set>
#include <deque>

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>

#include <sigc++/basic_signal.h>

namespace Eris
{

typedef std::list<std::string> StringList;	
typedef std::set<std::string> StringSet;

class BaseException : public std::exception
{
public:
	BaseException(const std::string &m) : _msg(m) {;}
	const std::string _msg;
};	
	
class InvalidOperation : public BaseException
{
public:
	InvalidOperation(const std::string &m) : BaseException(m) {;}
};
	
/// throw when processing encounters an Atlas message that deviates from the spec

class IllegalMessage : public BaseException
{
public:
	IllegalMessage(const Atlas::Message::Object &m, const std::string &s) :
		BaseException(s), what(m) {;}
			
	Atlas::Message::Object what;
};

/// Thrown when processing an Atlas::Object that isn't what we expected
class IllegalObject : public BaseException
{
public:
	IllegalObject(const Atlas::Objects::Root &o, const std::string &s) :
		BaseException(s), what(o) {;}
			
	Atlas::Objects::Root what;
};

class NetworkFailure : public BaseException
{
public:
	NetworkFailure(const std::string &s) :
		BaseException(s) {;}
};


typedef std::list<Atlas::Message::Object> MessageList;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Coord
{
public:
	Coord(const Atlas::Message::Object &obj);
	Coord(double a, double b, double c) :
		x(a), y(b), z(c)
	{;}
	
	Atlas::Message::Object asObject() const;
		
	double x,y,z;
};
	
typedef Coord BBox;

// Forward Decls
class Entity;
typedef Entity* EntityPtr;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
	#include <winsock.h>
	typedef SOCKET Socket;
#else
	typedef int Socket;
#endif

// would prefer a set (for faster find() impl), but that assumed an ordering
// operation on Sockets; this is fine for Unix, but less certain for Win32
typedef std::list<Socket> SocketList;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// A Generic 'event' occurred signal, especially for use with Wait exceptions
typedef SigC::Signal0<void> Signal;

/** OperationBlocked exceptions are thrown by certain Eris functions when they cannot
proceed (almost always becuase they are waiting on data from the server).  They
are deliberately placed outside the standard exception heirarchy so people
don't accidently catch them.

The member signal is emitted when the original operation can be restarted
(i.e the blocking condition no longer exists). The major complication is storing sufficent
data to restart the request. */

class OperationBlocked
{
public:
	OperationBlocked(Signal &rsig);
	Signal& _continue;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};

#endif
