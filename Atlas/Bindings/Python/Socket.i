class Socket
{
public:
		/// construct a disconnected socket
		Socket() { sock = (SOCKET)-1; }
		/// construct from an open socket handle
		//Socket( SOCKET asock ) { sock = asock; }
		/// destructor
virtual		~Socket() {}
                
		/// server socket startup
virtual int	listen(const std::string& addr, int port, int backlog);
		/// accept a new connection from a server socket
virtual Socket*	accept() { return 0; }
                
		/// connect to a remote host
virtual int	connect(const std::string& addr, int port);
                
		/// send data over socket
virtual int	send    (const std::string& data);
		/// send data to a spcific destination (UDP Support)
virtual int	sendTo  (const std::string& data, const struct sockaddr_in& addr);
                
		/// recieve data from socket
virtual int	recv    (std::string& buf);
		/// recieve data with source information (UDP Support)
virtual int	recvFrom(std::string& buf, const struct sockaddr_in& addr);	
                
		/// shut down the connection
virtual void	close();
                
		/// get operating system socket handle
virtual SOCKET	getSock();
};
