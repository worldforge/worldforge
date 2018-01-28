#include "Exceptions.h"

namespace Eris {

InvalidAtlas::InvalidAtlas(const std::string& msg, const Atlas::Objects::Root& obj) noexcept:
    BaseException(msg),
    m_obj(obj)
{
}

InvalidAtlas::InvalidAtlas(const std::string& msg, const Atlas::Message::Element&) noexcept:
    BaseException(msg)
{
}

}
