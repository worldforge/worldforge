
#ifndef __AtlasSocket_h_
#define __AtlasSocket_h_

#include <string>
using std::string;

#ifdef __linux__
	#include <sys/time.h>
	#include <time.h>
	#include <sys/socket.h>
	#include <sys/select.h>
	#include <sys/ioctl.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#define SOCKET int
	#define sinlen_t size_t
#elif _WIN32
	#define Win32_Winsock
	#define FD_SETSIZE 2048
	#include <windows.h>
	#define sinlen_t int
#endif

namespace Atlas
{
/** Base Class for Stream Sockets

The Socket Class defines the basic client and server
socket interfaces need by the Client and Server classes.

@author John Barrett (ZW) <jbarrett@box100.com

@see Client
@see Server
@see TCPSocket

*/

class Socket
{
public:
			/// construct a disconnected socket
			Socket() { sock = (SOCKET)-1; }
			/// construct from an open socket handle
			Socket( SOCKET asock ) { sock = asock; }
			/// destructor
virtual			~Socket() {}

			/// server socket startup
virtual int		listen(const string& addr, int port, int backlog);
			/// accept a new connection from a server socket
virtual Socket*		accept() { return 0; }

			/// connect to a remote host
virtual int		connect(const string& addr, int port);

			/// send data over socket
virtual int		send    (const string& data);
			/// send data to a spcific destination (UDP Support)
virtual int		sendTo  (const string& data, const struct sockaddr_in& addr);

			/// recieve data from socket
virtual int		recv    (string& buf);
			/// recieve data with source information (UDP Support)
virtual int		recvFrom(string& buf, const struct sockaddr_in& addr);	

			/// shut down the connection
virtual void		close();

			/// get operating system socket handle
virtual SOCKET		getSock();

protected:

			/// system socket handle
SOCKET			sock;
			/// recieve data buffer
string			rbuf;

};

} // namespace Atlas
#endif
