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


class NetworkFailure : public BaseException
{
public:
	NetworkFailure(const std::string &s) :
		BaseException(s) {;}
        virtual ~NetworkFailure() throw();
};

}

#endif
