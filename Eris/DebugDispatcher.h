#ifndef ERIS_DEBUG_DISPATCH_H
#define ERIS_DEBUG_DISPATCH_H

#include <Eris/Dispatcher.h>

#include <iostream>
#include <fstream>

namespace Atlas {
  namespace Message { class Encoder; }
  template <class Stream> class Codec;
}

namespace Eris
{

class DebugDispatcher : public LeafDispatcher
{
public:
	DebugDispatcher(const std::string &logFile);	
	virtual ~DebugDispatcher();

	virtual bool dispatch(DispatchContextDeque &dq);

protected:
	Atlas::Codec<std::iostream> *_codec;
	Atlas::Message::Encoder *_enc;
	std::fstream _log;
};
	
} // of namespace Eris

#endif
