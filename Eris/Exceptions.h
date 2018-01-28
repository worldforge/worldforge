#ifndef ERIS_EXCEPTIONS_H
#define ERIS_EXCEPTIONS_H

#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/SmartPtr.h>

#include <string>
#include <stdexcept>

namespace Eris
{

/**
This is the Eris base for all exceptions; note it inherits from std::except,
which isn't ideal. One option would be to refactor the various final
exceptions so they inherit from the 'closest' ISO C++ exception, but it
hardly seems worth it.
*/
class BaseException : public std::runtime_error
{
public:
	explicit BaseException(const std::string &m) noexcept:
		std::runtime_error(m), _msg(m) {;}

	~BaseException() noexcept override = default;
	const std::string _msg;
};	
	
class InvalidOperation : public BaseException
{
public:
	explicit InvalidOperation(const std::string &m) noexcept:
			BaseException(m) {;}

	~InvalidOperation() noexcept override = default;
};

/// Exception used to indicated malformed or unexpected Atlas from the server
class InvalidAtlas : public BaseException
{
public:
    InvalidAtlas(const std::string& msg, const Atlas::Objects::Root& obj) noexcept;
    
    InvalidAtlas(const std::string& msg, const Atlas::Message::Element& el) noexcept;

	~InvalidAtlas() noexcept override = default;
private:
    Atlas::Objects::Root m_obj;
};

class NetworkFailure : public BaseException
{
public:
	explicit NetworkFailure(const std::string &s) noexcept:
		BaseException(s) {;}

	~NetworkFailure() noexcept override = default;
};

}

#endif
