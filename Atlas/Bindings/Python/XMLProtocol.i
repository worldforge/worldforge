class XMLProtocol: public Protocol
{
public:
	/// constructor
	XMLProtocol();
	/// destructor
	virtual ~XMLProtocol();
	/// create a new instance of XMLProtocol
	Protocol*  makenew();
};
