// This file is distributed under the GNU Lesser General Public license.
// Copyright (C) 2000 Dmitry Derevyanko (See the file COPYING for details).

#include "GzipFilter.h"

#ifndef ASSERT
#include <assert.h>
#define ASSERT(exp) assert(exp)
#endif

namespace Atlas {

  GzipFilter::GzipFilter(int level)
  {
    incoming.next_in = Z_NULL;
    incoming.avail_in = 0;
    incoming.zalloc = Z_NULL;
    incoming.zfree = Z_NULL;
  
    outgoing.zalloc = Z_NULL;
    outgoing.zfree = Z_NULL;
  
    inflateInit(&incoming);
    deflateInit(&outgoing, level);
  }
  
  GzipFilter::~GzipFilter(void)
  {
    inflateEnd(&incoming);
    deflateEnd(&outgoing);
  }

    
  string GzipFilter::encode(const string& data)
  {
    string out_string;
    int status;
    
    //cout << "encoding [" 
    //<< data.data() 
    // << "] " << data.size() << endl;

    buf[0] = '\0';

    outgoing.next_in = (unsigned char *)data.data();
    outgoing.avail_in = data.size();

    do 
      {       
	outgoing.next_out = buf;
	outgoing.avail_out = sizeof(buf);
	  
	status = deflate(&outgoing, Z_SYNC_FLUSH);
	  
	ASSERT(status == Z_OK);
	  
	out_string.append((char*)buf, sizeof(buf) - outgoing.avail_out);	
	  
      } while(outgoing.avail_out == 0);
    
    //cout << "out [" 
    // << "] " << out_string.size() << endl;
      
    return out_string;
  }
    
  string GzipFilter::decode(const string& data)
  {
    string out_string;
    int status;

    //cout << "decoding ["
    //	 << "] " << data.size() << endl;

    buf[0] = '\0';

    incoming.next_in = (unsigned char*)data.data();
    incoming.avail_in = data.size();

    do 
      {
	incoming.next_out = buf;
	incoming.avail_out = sizeof(buf);
	  
	status = inflate(&incoming, Z_SYNC_FLUSH);
	  
	ASSERT(status == Z_OK);
	  
	out_string.append((char*)buf, sizeof(buf) - incoming.avail_out);
	  
      } while(incoming.avail_out == 0);

    //cout << "out [" 
    //<<  out_string 
    //<< "] " << out_string.size() << endl;

    return out_string;
  }

} // namespace Atlas
