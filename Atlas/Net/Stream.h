// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Michael Day, Dmitry Derevyanko, Stefanus Du Toit

#ifndef ATLAS_NET_STREAM_H
#define ATLAS_NET_STREAM_H

#include <Atlas/Negotiate.h>

#include <iosfwd>
#include <string>
#include <list>

namespace Atlas { 

class Bridge;

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

    NegotiateHelper(std::list<std::string> & names);

    bool get(std::string &buf, const std::string & header);
    void put(std::string &buf, const std::string & header);

  private:

    std::list<std::string> & m_names;

  };

class StreamConnect : public Atlas::Negotiate
{
    public:

    StreamConnect(const std::string& name, std::iostream&, Atlas::Bridge*);

    virtual ~StreamConnect();

    virtual void poll(bool can_read = true);

    virtual State getState();
    virtual Atlas::Codec * getCodec();

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
    std::iostream& m_socket;
    Atlas::Bridge* m_bridge;
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
 
class StreamAccept : public Atlas::Negotiate
{
    public:

    StreamAccept(const std::string& name, std::iostream&, Atlas::Bridge*);

    virtual ~StreamAccept();

    virtual void poll(bool can_read = true);

    virtual State getState();
    virtual Atlas::Codec * getCodec();

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
    std::iostream& m_socket;
    Atlas::Bridge* m_bridge;
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

