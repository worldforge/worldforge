// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Michael Day, Stefanus Du Toit

#ifndef ATLAS_FILTER_H
#define ATLAS_FILTER_H

#include "config.h"

#include <string>

#ifdef HAVE_STREAMBUF
#include <streambuf>
#else
#include <streambuf.h>
#endif

#include <cstring>
// for memcpy

namespace Atlas {

/** Atlas stream filter

Filters are used by Codec to transform the byte stream before transmission.
The transform must be invertible; that is to say, encoding a string and then
decoding it must result in the original string. Filters can be used for
compression, encryption or performing checksums and other forms of
transmission error detection. A compound filter can be created that acts like
a single filter, allowing various filters to be chained together in useful
ways such as compressing and then encrypting.

@see Codec
@see Negotiate
*/

class Filter
{
    public:
   
    Filter(Filter* = 0);
    virtual ~Filter();

    virtual void begin() = 0;
    virtual void end() = 0;

    virtual std::string encode(const std::string&) = 0;
    virtual std::string decode(const std::string&) = 0;

    enum Type
    {
	CHECKSUM,
	COMPRESSION,
	ENCRYPTION,
    };

    protected:

    Filter* next;
};

#ifdef HAVE_STREAMBUF
class filterbuf : public std::streambuf {
#else
typedef int int_type;

class filterbuf : public streambuf {
#endif
public:

  filterbuf(std::streambuf& buffer,
            Filter& filter)
   : m_streamBuffer(buffer), m_filter(filter)
  {
    setp(m_outBuffer, m_outBuffer + (m_outBufferSize - 1));
    setg(m_inBuffer + m_inPutback, m_inBuffer + m_inPutback,
         m_inBuffer + m_inPutback);
  }
  
  virtual ~filterbuf()
  {
    sync();
  }
  
protected:
  static const int m_outBufferSize = 10;
  char m_outBuffer[m_outBufferSize];

  static const int m_inBufferSize = 10;
  static const int m_inPutback = 4;
  char m_inBuffer[m_inBufferSize];

  int flushOutBuffer()
  {
    int num = pptr() - pbase();
    std::string encoded = m_filter.encode(std::string(pbase(), pptr()));
    m_streamBuffer.sputn(encoded.c_str(), encoded.size());
    pbump(-num);
    return num;
  }  
  
  virtual int_type overflow(int_type c)
  {
    if (c != EOF) {
      *pptr() = c;
      pbump(1);
    }
    if (flushOutBuffer() == EOF) return EOF;
    return c;
  }

  virtual int_type underflow()
  {
    if (gptr() < egptr()) return *gptr();
    
    int numPutback = gptr() - eback();

    if (numPutback > m_inPutback) numPutback = m_inPutback;

    std::memcpy(m_outBuffer + (m_inPutback - numPutback),
                gptr() - numPutback,
                numPutback);

    int num;

    //     FIXME
    // Here we need to actually
    //  * get data from m_streamBuffer
    //  * encode it with m_filter
    //  * put _that_ into the buffer
    //
    // Currently it just fetches it and places it straight in the
    // buffer.
    // The problem is the limited size of the buffer with the
    // Filter::decode operation not having any kind of size
    // limitation.
    num = m_streamBuffer.sgetn(m_inBuffer + m_inPutback,
                               m_inBufferSize - m_inPutback);
    if (num <= 0) return EOF;

    setg(m_inBuffer + (m_inPutback - numPutback),
         m_inBuffer + m_inPutback,
         m_inBuffer + m_inPutback + num);

    return *gptr();
  }
  
  virtual int sync()
  {
    if (flushOutBuffer() == EOF) return -1;
    return 0;
  }
  
private:

  std::streambuf& m_streamBuffer;
  Filter& m_filter;
};
 
} // Atlas namespace

#endif
