#ifndef ERIS_TYPES_H
#define ERIS_TYPES_H

#include <Atlas/Objects/Root.h>

#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/quaternion.h>
#include <wfmath/axisbox.h>
#include <wfmath/stream.h>

#include <sigc++/object.h>
#if SIGC_MAJOR_VERSION == 1 && SIGC_MINOR_VERSION == 0
#include <sigc++/basic_signal.h>
#else
#include <sigc++/signal.h>
#endif

// system headers
#include <vector>
#include <string>
#include <list>
#include <set>
#include <deque>

#include <stdexcept>

namespace Eris
{

typedef std::list<std::string> StringList;	
typedef std::set<std::string> StringSet;

/** This is the Eris base for all exceptions; note it inherits from std::except,
which isn't ideal. One option would be to refactor the various final
exceptions so they inherit from the 'closest' ISO C++ exception, but it
hardly seems worth it. */
class BaseException : public std::runtime_error
{
public:
	BaseException(const std::string &m) : 
		std::runtime_error(m), _msg(m) {;}
        virtual ~BaseException() throw() { }
	const std::string _msg;
};	
	
class InvalidOperation : public BaseException
{
public:
	InvalidOperation(const std::string &m) : BaseException(m) {;}
        virtual ~InvalidOperation() throw() { }
};
	
/// throw when processing encounters an Atlas message that deviates from the spec

class IllegalMessage : public BaseException
{
public:
	IllegalMessage(const Atlas::Message::Element &m, const std::string &s) :
		BaseException(s), what(m) {;}
        virtual ~IllegalMessage() throw() { }
			
	Atlas::Message::Element what;
};

/// Thrown when processing an Atlas::Object that isn't what we expected
class IllegalObject : public BaseException
{
public:
	IllegalObject(const Atlas::Objects::Root &o, const std::string &s) :
		BaseException(s), what(o) {;}
        virtual ~IllegalObject() throw() { }
			
	Atlas::Objects::Root what;
};

class NetworkFailure : public BaseException
{
public:
	NetworkFailure(const std::string &s) :
		BaseException(s) {;}
        virtual ~NetworkFailure() throw() { }
};


typedef std::list<Atlas::Message::Element> MessageList;


// Forward Decls
class Entity;
typedef Entity* EntityPtr;

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
	OperationBlocked(Signal &rsig) : 
		_continue(rsig)
	{;}
	
	Signal& _continue;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}	

#endif
