/**
 Worldforge Next Generation MetaServer

 Copyright (C) 2011 Sean Ryan <sryan@evercrack.com>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 */

/**
 * Assumptions:
 *		1) Any errors, or misbehaviour of any kind will be ignored / dropped
 *
 * 	NMT_NULL	Indicates that no response needs to be sent back
 * 		Type Size: 4 bytes
 * 		Value: 0
 * 		Payload Size: 0
 *
 * 	NMT_SERVERKEEPALIVE		indicates a keep alive for a server, also serves as a "registration"
 * 		Type Size: 4 bytes
 * 		Value: 1 ( 00 00 00 01 )
 * 		Payload Size: 0
 *
 * 	NMT_HANDSHAKE			Auth request
 * 		Type Size: 4 bytes
 * 		Value: 3 ( 00 00 00 03 )
 * 		Payload Size: 4 bytes ( a random int )
 *
 * 	NMT_SERVERSHAKE			Auth response
 * 		Type Size: 4 bytes
 * 		Value: 4 ( 00 00 00 04 )
 * 		Payload Size: 4 bytes ( random int )
 *
 *	Use Case 1: Server Registration or Keep-alive.  Only servers with sessions can perform additional
 *	            requests, such as attribute registration/removal.
 *		1) Game Server sends a NMT_SERVERKEEPALIVE
 *		2) MS responds with a NMT_HANDSHAKE, contains a random number, creates a handshake
 *		3) Game Server responds with NMT_SERVERSHAKE, contains same random number AND handshake exists,
 *		   creates session ( if session exists, expiry time is adjusted )
 *
 *	Use Case 2: Game Server Shutdown / Quit
 *		1) Game Server sends a NMT_TERMINATE, if session exists, it is removed.
 *
 *	Use Case 3: Client Registration or Keep-alive.  Only clients with sessions can perform additional
 *				requests, such as LIST, FILTER, etc.
 *		1) Game Client sends a NMT_CLIENTKEEPALIVE
 *		2) MS responds with a NMT_HANDSHAKE, contains random number, creates a handshake
 *		3) Game Client responds with NMT_CLIENTSHAKE, contains same random number AND handshake exists,
 *		   creates a client session ( if session exists, expiry time is adjusted ).
 */
#ifndef METASERVERPROTOCOL_HPP_
#define METASERVERPROTOCOL_HPP_

typedef uint32_t NetMsgType;

static const NetMsgType NMT_NULL = 0;
static const NetMsgType NMT_SERVERKEEPALIVE = 1;
static const NetMsgType NMT_CLIENTKEEPALIVE = 2;
static const NetMsgType NMT_HANDSHAKE = 3;
static const NetMsgType NMT_SERVERSHAKE = 4;
static const NetMsgType NMT_CLIENTSHAKE = 5;
static const NetMsgType NMT_TERMINATE = 6;
static const NetMsgType NMT_LISTREQ = 7;
static const NetMsgType NMT_LISTRESP = 8;
static const NetMsgType NMT_PROTO_ERANGE = 9;
static const NetMsgType NMT_LAST = 10;



#endif /* METASERVERPROTOCOL_HPP_ */
