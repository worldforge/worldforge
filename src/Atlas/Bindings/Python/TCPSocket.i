class TCPSocket: public Socket
{
public:
	/// constructor
	TCPSocket();
	/// constructor given an open socket
	//TCPSocket(SOCKET asock);
	/// destructor
	~TCPSocket();
        
	/// establish a server socket
int	listen  (const std::string& addr, int port, int blog);
	/// accept new connections from a server socket
Socket*	accept();
        
	/// connect to a remote host
int	connect (const std::string& addr, int port);
	/// send data over the socket
int	send    (const std::string& data);
	/// recieve data from the socket
int	recv    (std::string& data);
};
