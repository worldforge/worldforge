// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Michael Day, Dmitry Derevyanko, Stefanus Du Toit

// $Id$

#ifndef ATLAS_NET_STREAM_H
#define ATLAS_NET_STREAM_H

#include <Atlas/Negotiate.h>

#include <iosfwd>
#include <string>
#include <list>

namespace Atlas { 

class Bridge;

/// The Atlas network communication namespace.
///
/// This namespace contains classes to handle establishing network connections.
namespace Net {

/** Negotiation of codecs and filters for an Atlas connection

non blocking negotiation of Codecs and Filters
requires a list of avalable Codecs and Filters,
along with the name of sender and a Socket

@see Connection
@see Codec
@see Filter
*/

  class NegotiateHelper {

  public:

	explicit NegotiateHelper(std::list<std::string> & names);

    bool get(std::string &buf, const std::string & header);
    void put(std::string &buf, const std::string & header);

  private:

    std::list<std::string> & m_names;

  };

/// Negotiation of clients building a connection to a remote system.
///
/// Used once a stream connection has been established to the server.
/// This class offers the server a list of Atlas::Codec types that the
/// client can understand, and then listens for the servers decision.
/// Once the server has told the client which Atlas::Codec to use,
/// negotiation is flagged as complete, and this object can be deleted.
class StreamConnect : public Atlas::Negotiate
{
    public:

    StreamConnect(const std::string& name, std::istream& inStream, std::ostream& outStream);

	~StreamConnect() override = default;

	void poll(bool can_read) override;

	State getState() override;

	Atlas::Codec * getCodec(Atlas::Bridge&) override;

    private:

    enum
    {
	SERVER_GREETING,
	CLIENT_GREETING,
	CLIENT_CODECS,
	SERVER_CODECS,
	// CLIENT_FILTERS,
	// SERVER_FILTERS,
	DONE
    };

    int m_state;

    std::string m_outName;
    std::string m_inName;
    std::istream& m_inStream;
    std::ostream& m_outStream;
    std::list<std::string> m_inCodecs;
    std::list<std::string> m_inFilters;
  
    NegotiateHelper m_codecHelper;
    NegotiateHelper m_filterHelper;
    std::string m_buf;

    void processServerCodecs();
    void processServerFilters();

    //void processClientCodecs();
    //void processClientFilters();

    bool m_canPacked;
    bool m_canXML;
    bool m_canBach;

    bool m_canGzip;
    bool m_canBzip2;
};
 
/// Negotiation of servers accepting a connection from a remote system.
///
/// Used once a stream connection has been established by a client.
/// This class listens to the list of Atlas::Codec types that the
/// client offers, and then responds with the name of the Atlas::Codec
/// which it thinks is most suitable.
/// Once the server has told the client which Atlas::Codec to use,
/// negotiation is flagged as complete, and this object can be deleted.
class StreamAccept : public Atlas::Negotiate
{
    public:

    StreamAccept(const std::string& name, std::istream& inStream, std::ostream& outStream);

	~StreamAccept() override = default;

	void poll(bool can_read) override;

	State getState() override;

	Atlas::Codec * getCodec(Atlas::Bridge&) override;

    private:

    enum
    {
	SERVER_GREETING,
	CLIENT_GREETING,
	CLIENT_CODECS,
	SERVER_CODECS,
	CLIENT_FILTERS,
	SERVER_FILTERS,
	DONE
    };

    int m_state;

    std::string m_outName;
    std::string m_inName;
    std::istream& m_inStream;
    std::ostream& m_outStream;
    std::list<std::string> m_inCodecs;
    std::list<std::string> m_inFilters;
  
    NegotiateHelper m_codecHelper;
    NegotiateHelper m_filterHelper;
    std::string m_buf;

    //void processServerCodecs();
    //void processServerFilters();

    void processClientCodecs();
    void processClientFilters();

    bool m_canPacked;
    bool m_canXML;
    bool m_canBach;

    bool m_canGzip;
    bool m_canBzip2;
};

} } // namespace Atlas::Net

#endif

