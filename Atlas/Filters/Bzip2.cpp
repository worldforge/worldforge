// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit, Dmitry Derevyanko

// $Id$

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(HAVE_BZLIB_H) && defined(HAVE_LIBBZ2)

#include <Atlas/Filters/Bzip2.h>

#ifndef ASSERT
#include <cassert>
#define ASSERT(exp) assert(exp)
#endif

using Atlas::Filters::Bzip2;

const int BS100K = 6;
const int WORKFACTOR = 30;

void Bzip2::begin()
{
    incoming.next_in = NULL;
    incoming.avail_in = 0;
    incoming.bzalloc = NULL;
    incoming.bzfree = NULL;
    incoming.opaque = NULL;
  
    outgoing.bzalloc = NULL;
    outgoing.bzfree = NULL;
    outgoing.opaque = NULL;
  
    BZ2_bzCompressInit(&outgoing, BS100K, 0, WORKFACTOR);
    BZ2_bzDecompressInit(&incoming, 0, 0);
}
  
void Bzip2::end()
{
    BZ2_bzCompressEnd(&outgoing);
    BZ2_bzDecompressEnd(&incoming);
}
    
std::string Bzip2::encode(const std::string& data)
{
    std::string out_string;
    int status;
    
    buf[0] = 0;

    outgoing.next_in = (char*)data.data();
    outgoing.avail_in = data.size();

    do 
    {       
        outgoing.next_out = buf;
        outgoing.avail_out = sizeof(buf);
  
        status = BZ2_bzCompress(&outgoing, BZ_FLUSH);
  
        ASSERT(status != BZ_SEQUENCE_ERROR);

        if (status != BZ_SEQUENCE_ERROR) {
            out_string.append((char*)buf, sizeof(buf) - outgoing.avail_out);
        }
        // FIXME do something else in case of error?
    } while (outgoing.avail_out == 0);
    
    return out_string;
}
    
std::string Bzip2::decode(const std::string& data)
{
    std::string out_string;
    int status;

    buf[0] = 0;

    incoming.next_in = (char*)data.data();
    incoming.avail_in = data.size();

    do
    {
        incoming.next_out = buf;
        incoming.avail_out = sizeof(buf);

        status = BZ2_bzDecompress(&incoming);

        ASSERT(status == BZ_OK);

        if (status != BZ_SEQUENCE_ERROR) {
            out_string.append((char*)buf, sizeof(buf) - incoming.avail_out);
        }

    } while(incoming.avail_out == 0);

    return out_string;
}

#endif // HAVE_BZLIB_H && HAVE_LIBBZ2
