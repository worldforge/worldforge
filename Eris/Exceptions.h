#ifndef ERIS_EXCEPTIONS_H
#define ERIS_EXCEPTIONS_H

#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/SmartPtr.h>

#include <string>
#include <stdexcept>

namespace Eris
{

/**
This is the Eris base for all exceptions.
*/
class BaseException : public std::runtime_error
{
public:
	explicit BaseException(const std::string& m) noexcept:
		std::runtime_error(m) {;}

	~BaseException() noexcept override = default;
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
    InvalidAtlas(const std::string& msg) noexcept :
		BaseException(msg){}
    

	~InvalidAtlas() noexcept override = default;
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
