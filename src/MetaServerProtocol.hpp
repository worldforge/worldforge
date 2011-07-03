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
 * 	NMT_NULL
 * 		Description: empty packet
 * 		Type Size: 4 bytes
 * 		Value: 0
 * 		Payload Size: 0
 *
 * 	NMT_SERVERKEEPALIVE
 * 		Description: A game server keep-alive packet, part 1 of a 3-packet handshake
 * 		Type Size: 4 bytes
 * 		Value: 1 ( 00 00 00 01 )
 * 		Payload Size: 0
 *
 * 	NMT_HANDSHAKE
 * 		Description: The metaserver response to a SERVERKEEPALIVE or CLIENTKEEPALIVE, part 2 of 3-packet handshake
 * 		             Contains a random number.
 * 		Type Size: 4 bytes
 * 		Value: 3 ( 00 00 00 03 )
 * 		Payload Size: 4 bytes ( a random int )
 *
 * 	NMT_SERVERSHAKE
 * 		Description: The game server response to a HANDSHAKE, part 3 of 3-packet handshake.
 * 					 Triggers a new game server sesssion to be created, or updated.
 * 		Type Size: 4 bytes
 * 		Value: 4 ( 00 00 00 04 )
 * 		Payload Size: 4 bytes ( random int )
 *
 * 	NMT_CLIENTKEEPALIVE
 * 		Description: The game client keep-alive packet, part 1 of 3-packet handshake
 *		Type Size: 4 bytes
 *		Value: 2 ( 00 00 00 02 )
 *		Payload Size: 0
 *
 *	NMT_CLIENTSHAKE
 *		Description: The game client response to a HANDSHAKE, part 3 of a 3-packet handshake
 *		Type Size: 4 bytes
 *		Value: 5 ( 00 00 00 05 )
 * 		Payload Size: 4 bytes ( random int )
 *
 * 	NMT_LISTREQ
 * 		Description: A client request to give a list of servers
 * 		Type Size: 4 bytes
 * 		Value: 7 ( 00 00 00 07 )
 * 		Payload Size: 4 bytes ( int of the offset )
 *
 * 	NMT_LISTRESP
 * 		Description: The metaserver response with the values of IPs.
 * 		Type Size: 4 bytes
 * 		Value: 8 ( 00 00 00 08 )
 * 		Payload Size: Variable
 * 			4 bytes - int total servers sent
 * 			4 bytes - int packed servers sent
 * 			4 bytes - a 4 byte block representing EACH IP of the servers.  So if packed was set to 6,
 * 			          this block would be 24 bytes long.
 *
 *Example Use Cases:
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
/**
 *  Enhancements of the metaserver-ng
 */
static const NetMsgType NMT_SERVERATTR = 11;
static const NetMsgType NMT_CLIENTATTR = 12;
static const NetMsgType NMT_CLIENTFILTER = 13;
static const NetMsgType NMT_ATTRRESP = 14;
static const NetMsgType NMT_SERVERCLEAR = 15;
static const NetMsgType NMT_CLIENTCLEAR = 16;


#endif /* METASERVERPROTOCOL_HPP_ */
