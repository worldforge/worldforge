#ifndef ERIS_LOGSTREAM_H
#define ERIS_LOGSTREAM_H

#include "Log.h"

#include <Atlas/Objects/ObjectsFwd.h>
#include <Atlas/Message/Element.h>

#include <fmt/ostream.h>

namespace Atlas::Message {
class Element;
}

template<>
struct fmt::formatter<Atlas::Objects::Root> : ostream_formatter {
};
template<>
struct fmt::formatter<Atlas::Objects::Operation::RootOperation> : ostream_formatter {
};
template<>
struct fmt::formatter<Atlas::Message::Element> : ostream_formatter {
};

namespace Atlas {
namespace Objects {
std::ostream& operator<<(std::ostream& s, const Atlas::Objects::Root& obj);

}
namespace Message {
std::ostream& operator<<(std::ostream& s, const Atlas::Message::Element& msg);

}
}
#endif
