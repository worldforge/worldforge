#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <fstream>

#include <Atlas/Codecs/XML.h>
#include <Atlas/Message/Encoder.h>

#include <Eris/DebugDispatcher.h>

namespace Eris
{
	
DebugDispatcher::DebugDispatcher(const std::string &logFile) :
	LeafDispatcher("debug")
{
	_log.open(logFile.c_str(), std::ios::out | std::ios::app);
	_log << "=======================================" << std::endl;	
	_codec = new Atlas::Codecs::XML(_log, NULL);
	_enc = new Atlas::Message::Encoder(_codec);
}
	
DebugDispatcher::~DebugDispatcher()
{
	delete _enc;
	delete _codec;
	_log.close();
}

bool DebugDispatcher::dispatch(DispatchContextDeque &dq)
{
	_enc->streamMessage(dq.front());
	_log << std::endl;
	return false;
}

}
