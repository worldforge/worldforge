/*
        begin           : 1999.11.29
        copyright       : (C) 1999 by John Barrett (ZW)
        email           : jbarrett@box100.com

changes:

23 Jan 2000 - fex
    desctuctor added, server responsible for killing its ASocket
    source deps removed
*/
#ifndef __AtlasServer_h_
#define __AtlasServer_h_

#include "Socket.h"
#include "Client.h"

namespace Atlas
{
/** Server Class for managing multiple Client connections.
<p>The Server class provides facilities for managing multiple
inbound and outbound client connections. The Server Class is
both a listener for new inbound connections and a multiplexor
for incoming data on multiple streams.

<p>Responsible for killing its listener socket on destruction

@see Client
@see Socket
*/

class Server
{
public:

	/// start listening for connections on an established socket
	Server(Socket* listener);
virtual ~Server();
	/// process data events and test for new connections
	int poll();
	/// process data events and test for new connections
	int poll(long usec);

	/// retrieve a new connection
	Socket* accept();

	/// add a client from the data ready event list
	int addClient(Client* client);
	/// remove a client from the data ready event list
	int delClient(Client* client);

	/// add a client from the ready to send list
	int addClientSend(Client* client);
	/// remove a client from the ready to send list
	int delClientSend(Client* client);

private:
    Server( const Server& );
    /// listener socket
    Socket*    lsock;
    /// OS file descriptor for listener socket
    int         slsock;
    /// File Descriptor to Client Object mapping
    Client*	csock[FD_SETSIZE];
    int		asock[FD_SETSIZE];

    fd_set  fdread;
    fd_set  fdsend;
    fd_set  fderrs;
};

} // namespace Atlas

#endif

