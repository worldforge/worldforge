#ifndef ERIS_EXCEPTIONS_H
#define ERIS_EXCEPTIONS_H

#include <Atlas/Objects/Root.h>

#include <sigc++/signal.h>

#include <string>

#include <stdexcept>

namespace Eris
{

/** This is the Eris base for all exceptions; note it inherits from std::except,
which isn't ideal. One option would be to refactor the various final
exceptions so they inherit from the 'closest' ISO C++ exception, but it
hardly seems worth it. */
class BaseException : public std::runtime_error
{
public:
	BaseException(const std::string &m) : 
		std::runtime_error(m), _msg(m) {;}
        virtual ~BaseException() throw();
	const std::string _msg;
};	
	
class InvalidOperation : public BaseException
{
public:
	InvalidOperation(const std::string &m) : BaseException(m) {;}
        virtual ~InvalidOperation() throw();
};
	
/// throw when processing encounters an Atlas message that deviates from the spec

class IllegalMessage : public BaseException
{
public:
	IllegalMessage(const Atlas::Message::Element &m, const std::string &s) :
		BaseException(s), what(m) {;}
        virtual ~IllegalMessage() throw();
			
	Atlas::Message::Element what;
};

/// Thrown when processing an Atlas::Object that isn't what we expected
class IllegalObject : public BaseException
{
public:
	IllegalObject(const Atlas::Objects::Root &o, const std::string &s) :
		BaseException(s), what(o) {;}
        virtual ~IllegalObject() throw();
			
	Atlas::Objects::Root what;
};

class NetworkFailure : public BaseException
{
public:
	NetworkFailure(const std::string &s) :
		BaseException(s) {;}
        virtual ~NetworkFailure() throw();
};

class UnknownEntity : public BaseException
{
public:
	UnknownEntity(const std::string &msg, const std::string &id) :
		BaseException(msg), _id(id) {;}
	virtual ~UnknownEntity() throw();
	std::string _id;
};

class UnknownProperty : public InvalidOperation
{
public:	
	UnknownProperty(const std::string &p, const std::string &m) :
		InvalidOperation(m), prop(p)
	{;}
        virtual ~UnknownProperty() throw();
	
	const std::string prop;
};

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
