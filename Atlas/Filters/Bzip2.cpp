// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit, Dmitry Derevyanko

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(HAVE_BZLIB_H) && defined(HAVE_LIBBZ2)

#include "../Stream/Filter.h"

#include <cstdio>
#include <bzlib.h>

#ifndef ASSERT
#include <cassert>
#define ASSERT(exp) assert(exp)
#endif

using namespace std;
using namespace Atlas::Stream;

const int BS100K = 6;
const int WORKFACTOR = 30;

class Bzip2 : public Filter
{
    bz_stream incoming;
    bz_stream outgoing;
    char buf[4096];

    public:

    virtual void Begin();
    virtual void End();
    
    virtual string Encode(const string&);
    virtual string Decode(const string&);
};

namespace
{
    Filter::Factory<Bzip2> factory("BZIP2", Filter::Metrics(Filter::COMPRESSION));
}

void Bzip2::Begin()
{
    incoming.next_in = NULL;
    incoming.avail_in = 0;
    incoming.bzalloc = NULL;
    incoming.bzfree = NULL;
    incoming.opaque = NULL;
  
    outgoing.bzalloc = NULL;
    outgoing.bzfree = NULL;
    outgoing.opaque = NULL;
  
    bzCompressInit(&outgoing, BS100K, 0, WORKFACTOR);
    bzDecompressInit(&incoming, 0, 0);
}
  
void Bzip2::End()
{
    bzCompressEnd(&outgoing);
    bzDecompressEnd(&incoming);
}
    
string Bzip2::Encode(const string& data)
{
    string out_string;
    int status;
    
    buf[0] = 0;

    outgoing.next_in = (char*)data.data();
    outgoing.avail_in = data.size();

    do 
    {       
	outgoing.next_out = buf;
	outgoing.avail_out = sizeof(buf);
	  
	status = bzCompress(&outgoing, BZ_FLUSH);
	  
	ASSERT(status == BZ_OK); // not sure about this - it may be
                             // status != BZ_SEQUENCE_ERROR
	  
	out_string.append((char*)buf, sizeof(buf) - outgoing.avail_out);	
	  
    } while (outgoing.avail_out == 0);
    
    return out_string;
}
    
string Bzip2::Decode(const string& data)
{
    string out_string;
    int status;

    buf[0] = 0;

    incoming.next_in = (char*)data.data();
    incoming.avail_in = data.size();

    do 
    {
	incoming.next_out = buf;
	incoming.avail_out = sizeof(buf);
	  
	status = bzDecompress(&incoming);
	  
	ASSERT(status == BZ_OK);
    
	out_string.append((char*)buf, sizeof(buf) - incoming.avail_out);
	  
    } while(incoming.avail_out == 0);

    return out_string;
}

#endif // HAVE_BZLIB_H && HAVE_LIBBZ2
