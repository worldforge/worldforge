#ifndef __AtlasNet_h__
#define __AtlasNet_h__

#include "../Object/Debug.h"
#include "../Object/Types.h"
#include "../Object/Object.h"

#include "Protocol.h"
#include "ProtocolDecoder.h"
#include "ProtocolEncoder.h"
#include "Codec.h"
#include "Compressor.h"

#include "PackedAsciiDecoder.h"
#include "PackedAsciiEncoder.h"
#include "PackedAsciiProtocol.h"

#include "XMLDecoder.h"
#include "XMLEncoder.h"
#include "XMLProtocol.h"

#include "Socket.h"
#include "TCPSocket.h"

#include "Client.h"
#include "Server.h"

#ifdef PYTHON_INC_HACK
#include "Python.h"
#else
#include <python1.5/Python.h>
#endif

void initAtlasNet();

#endif
