#include <Eris/Exceptions.h>

namespace Eris {

BaseException::~BaseException() throw()
{
}

InvalidOperation::~InvalidOperation() throw()
{
}

NetworkFailure::~NetworkFailure() throw()
{
}

InvalidAtlas::InvalidAtlas(const std::string& msg, const Atlas::Objects::Root& obj) :
    BaseException(msg),
    m_obj(obj)
{
}

InvalidAtlas::~InvalidAtlas() throw()
{
}

}
