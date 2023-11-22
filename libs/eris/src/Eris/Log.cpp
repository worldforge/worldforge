#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "Log.h"

#include <Atlas/Message/MEncoder.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/PresentationBridge.h>


namespace Atlas {
namespace Objects {
std::ostream& operator<<(std::ostream& os, const Atlas::Objects::Root& obj)
{
    std::stringstream s;
	Atlas::PresentationBridge bridge(s);
    Atlas::Objects::ObjectsEncoder debugEncoder(bridge);
    debugEncoder.streamObjectsMessage(obj);
    return os << s.str();
}
}
namespace Message {
std::ostream& operator<<(std::ostream& os, const Atlas::Message::Element& msg)
{
	if (msg.isMap()) {
		std::stringstream s;
		Atlas::PresentationBridge bridge(s);
		Atlas::Message::Encoder debugEncoder(bridge);
		debugEncoder.streamMessageElement(msg.Map());
		return os << s.str();
	}
	return os;
}
}
}
namespace Eris {
std::shared_ptr<spdlog::logger> logger = std::make_shared<spdlog::logger>("eris");;
}