// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day, Dmitry Derevyanko

#ifndef ATLAS_NET_STREAM_H
#define ATLAS_NET_STREAM_H

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

  template <class T>
  class NegotiateHelper {

    typedef std::list<T*> Factories;

  public:

    NegotiateHelper(std::list<std::string> *names, Factories *out_factories);

    bool get(std::string &buf, std::string header);
    void put(std::string &buf, std::string header);

  private:

    std::list<std::string> *names;
    Factories *outFactories;

  };

class StreamConnect : public Atlas::Negotiate<iostream>
{
    public:

    StreamConnect(const std::string& name, iostream&, Atlas::Bridge*);

    virtual void Poll();
    virtual void Run();

    virtual State GetState();
    virtual Atlas::Connection<iostream> GetConnection();

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
    iostream& socket;
    Atlas::Bridge* bridge;
    std::list<std::string> inCodecs;
    std::list<std::string> inFilters;
  
    typedef std::list<Atlas::Factory<Atlas::Codec<iostream> >*> FactoryCodecs;
    typedef std::list<Atlas::Factory<Atlas::Filter>*> FactoryFilters;

    FactoryCodecs outCodecs;
    FactoryFilters outFilters;
    NegotiateHelper<Atlas::Factory<Atlas::Codec<iostream> > > codecHelper;
    NegotiateHelper<Atlas::Factory<Atlas::Filter > > filterHelper;
    std::string buf;

    void processServerCodecs();
    void processServerFilters();

    void processClientCodecs();
    void processClientFilters();
};
 
class StreamAccept : public Atlas::Negotiate<iostream>
{
    public:

    StreamAccept(const std::string& name, iostream&, Atlas::Bridge*);

    virtual void Poll();
    virtual void Run();

    virtual State GetState();
    virtual Atlas::Connection<iostream> GetConnection();

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
    iostream& socket;
    Atlas::Bridge* bridge;
    std::list<std::string> inCodecs;
    std::list<std::string> inFilters;
  
    typedef std::list<Atlas::Factory<Atlas::Codec<iostream> >*> FactoryCodecs;
    typedef std::list<Atlas::Factory<Atlas::Filter>*> FactoryFilters;

    FactoryCodecs outCodecs;
    FactoryFilters outFilters;
    NegotiateHelper<Atlas::Factory<Atlas::Codec<iostream> > > codecHelper;
    NegotiateHelper<Atlas::Factory<Atlas::Filter > > filterHelper;
    std::string buf;

    void processServerCodecs();
    void processServerFilters();

    void processClientCodecs();
    void processClientFilters();
};

} } // Atlas::Net

#endif

