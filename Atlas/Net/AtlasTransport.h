#ifndef __AtlasTransport_h__
#define __AtlasTransport_h__

#include <AtlasDebug.h>

#include <AtlasPython.h>
#include <AtlasTypes.h>
#include <AtlasObject.h>

#include <AtlasProtocol.h>
#include <AtlasProtocolDecoder.h>
#include <AtlasProtocolEncoder.h>
#include <AtlasCodec.h>
#include <AtlasCompressor.h>

#include <AtlasPackedAsciiDecoder.h>
#include <AtlasPackedAsciiEncoder.h>
#include <AtlasPackedAsciiProtocol.h>

#include <AtlasXMLDecoder.h>
#include <AtlasXMLEncoder.h>
#include <AtlasXMLProtocol.h>

#include <AtlasSocket.h>
#include <AtlasTCPSocket.h>

#include <AtlasClient.h>
#include <AtlasServer.h>

void initAtlasTransport();

#endif
