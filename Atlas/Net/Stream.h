// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Michael Day, Dmitry Derevyanko, Stefanus Du Toit

#ifndef ATLAS_NET_STREAM_H
#define ATLAS_NET_STREAM_H

#include <iosfwd>
#include <string>
#include <list>

#include "../Negotiate.h"
#include "../Filter.h"

namespace Atlas { namespace Net {

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

    NegotiateHelper(std::list<std::string> *names);

    bool get(std::string &buf, std::string header);
    void put(std::string &buf, std::string header);

  private:

    std::list<std::string> *names;

  };

class StreamConnect : public Atlas::Negotiate<std::iostream>
{
    public:

    StreamConnect(const std::string& name, std::iostream&, Atlas::Bridge*);

    virtual ~StreamConnect() {}

    virtual void poll(bool can_read = true);

    virtual State getState();
    virtual Atlas::Codec<std::iostream> * getCodec();

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

    int state;

    std::string outName;
    std::string inName;
    std::iostream& socket;
    Atlas::Bridge* bridge;
    std::list<std::string> inCodecs;
    std::list<std::string> inFilters;
  
    NegotiateHelper codecHelper;
    NegotiateHelper filterHelper;
    std::string buf;

    void processServerCodecs();
    void processServerFilters();

    //void processClientCodecs();
    //void processClientFilters();

    bool m_canPacked;
    bool m_canXML;

    bool m_canGzip;
    bool m_canBzip2;
};
 
class StreamAccept : public Atlas::Negotiate<std::iostream>
{
    public:

    StreamAccept(const std::string& name, std::iostream&, Atlas::Bridge*);

    virtual ~StreamAccept() {}

    virtual void poll(bool can_read = true);

    virtual State getState();
    virtual Atlas::Codec<std::iostream>* getCodec();

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

    int state;

    std::string outName;
    std::string inName;
    std::iostream& socket;
    Atlas::Bridge* bridge;
    std::list<std::string> inCodecs;
    std::list<std::string> inFilters;
  
    NegotiateHelper codecHelper;
    NegotiateHelper filterHelper;
    std::string buf;

    //void processServerCodecs();
    //void processServerFilters();

    void processClientCodecs();
    void processClientFilters();

    bool m_canPacked;
    bool m_canXML;

    bool m_canGzip;
    bool m_canBzip2;
};

} } // namespace Atlas::Net

#endif

