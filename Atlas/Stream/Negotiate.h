// This file may be redistributed and modified under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day, Dmitry Derevyanko

#ifndef ATLAS_STREAM_NEGOTIATE_H
#define ATLAS_STREAM_NEGOTIATE_H

#include "Codec.h"

namespace Atlas { namespace Stream {

  class FactoryName {
  public:

    FactoryName(std::string n) : name(n) { }

    std::string &GetName() { return name; }

  private:

    std::string name;

  };


  typedef std::list<Factory<Codec>*> FactoryCodecs;
  typedef std::list<Factory<Filter>*> FactoryFilters;
  typedef std::list<FactoryName> FactoryNames;


  template <class T>
  class NegotiateHelper {

    typedef std::list<T*> Factories;

  public:

    NegotiateHelper(FactoryNames *in_factories, Factories *out_factories);

    bool get(std::string &buf, std::string header);
    void put(std::string &buf, std::string header);

  private:

    FactoryNames *inFactories;
    Factories *outFactories;

  };

/** Negotiation of codecs and filters for an Atlas connection

non blocking negotiation of Codecs and Filters
requires a list of avalable Codecs and Filters,
along with the name of sender and a Socket

@see Codec
@see Filter

*/

class Negotiate
{
    public:

    Negotiate(std::string& name, Net::Socket*);

    bool done();

    void negotiateServer();
    void negotiateClient();

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
    Net::Socket* sock;
    FactoryNames inCodecs;
    FactoryCodecs outCodecs;
    FactoryNames inFilters;
    FactoryFilters outFilters;
    NegotiateHelper<Factory<Codec> > codecHelper;
    NegotiateHelper<Factory<Filter> > filterHelper;
    std::string buf;

    void processServerCodecs();
    void processServerFilters();

    void processClientCodecs();
    void processClientFilters();
};
 
} } // Atlas::Stream

#endif // ATLAS_STREAM_NEGOTIATE_H

