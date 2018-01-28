// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day, Dmitry Derevyanko

// $Id$

#include <Atlas/Net/Stream.h>

#include <Atlas/Codecs/XML.h>
#include <Atlas/Codecs/Packed.h>
#include <Atlas/Codecs/Bach.h>

#include <iostream>

#define Debug(prg) { if (debug_flag) { prg } }

static const bool debug_flag = false;

static std::string get_line(std::string &s, char ch)
{
  std::string out;
  int n = (int) s.find(ch);
  if(n > 0) 
    {
      out.assign(s, 0, (unsigned long) n);
      s.erase(0, n+1UL);
    }

  return out;
}

static inline std::string get_line(std::string &s1, char ch, std::string &s2)
{
  s2 = get_line(s1, ch);

  return s2;
}

namespace Atlas { namespace Net {

NegotiateHelper::NegotiateHelper(std::list<std::string> & names) :
  m_names(names)
{ 
}

bool NegotiateHelper::get(std::string &buf, const std::string & header)
{
  std::string s, h;
  
  while(!buf.empty())
    {
      // check for end condition
      if(buf.find('\n') == 0)
        {
          buf.erase(0, 1);
          return true;
        }
        
      if(get_line(buf, '\n', s) == "")
        break;
        
      if(get_line(s, ' ', h) == header)
        {
          m_names.push_back(s);
            
          Debug( std::cout << " got: " << s << std::endl; );
        }
      else
        Debug( std::cerr << "Unknown pattern " << h << std::endl; );
    }
  return false;
}

void NegotiateHelper::put(std::string &buf, const std::string & header)
{
  buf.erase();

  buf += header;
  buf += " Packed\n";

  buf += header;
  buf += " XML\n";

  buf += header;
  buf += " Bach\n";

  buf += header;
  buf += " Gzip\n";

  buf += header;
  buf += " Bzip2\n";

  buf += "\n";
}

StreamConnect::StreamConnect(const std::string& name, std::istream& inStream, std::ostream& outStream) :
  m_state(SERVER_GREETING), m_outName(name), m_inStream(inStream), m_outStream(outStream),
  m_codecHelper(m_inCodecs), m_filterHelper(m_inFilters),
  m_canPacked(true), m_canXML(true), m_canBach(true),m_canGzip(true), m_canBzip2(true)
{
}

void StreamConnect::poll(bool can_read)
{
    Debug( std::cout << "** Client(" << m_state << ") : " << m_inStream.rdbuf()->in_avail() << std::endl; );

    std::string out;

    if (can_read) {
        // Cause the stream to read from its socket
        m_inStream.peek();
    }

    std::streamsize count;
    while ((count = m_inStream.rdbuf()->in_avail()) > 0) {
        for (int i = 0 ; i < count; ++i) {
            m_buf += (char) m_inStream.rdbuf()->sbumpc();
        }
    }

    if(m_state == SERVER_GREETING)
    {
        // get server greeting

        if (m_buf.size() > 0 && get_line(m_buf, '\n', m_inName) != "")
        {
            Debug( std::cout << "server: " << m_inName << std::endl; );
            m_state = CLIENT_GREETING;
        }
    }

    if(m_state == CLIENT_GREETING)
    {
        // send client greeting
        
        m_outStream << "ATLAS " << m_outName << std::endl;
        m_state = CLIENT_CODECS;
    }
    
    if (m_state == CLIENT_CODECS)
    {
        //processClientCodecs();
        m_codecHelper.put(out, "ICAN");
        m_outStream << out << std::flush;
        m_state = SERVER_CODECS;
    }

    if(m_state == SERVER_CODECS)
    {
        if (m_codecHelper.get(m_buf, "IWILL"))
        {
            processServerCodecs();
            m_state = DONE;
        }
    }
    
#if 0
    if (m_state == CLIENT_FILTERS)
    {
        //processClientFilters();
        m_filterHelper.put(out, "ICAN");
        m_socket << out << std::flush;
        m_state++;
    }
    
    if (m_state == SERVER_FILTERS)
    {
        if (m_filterHelper.get(m_buf, "IWILL"))
        {
            processServerFilters();
            m_state++;
        }
    }
#endif
}

Atlas::Negotiate::State StreamConnect::getState()
{
    if (m_state == DONE)
    {
        if (m_canPacked || m_canXML || m_canBach)
        {
            return SUCCEEDED;
        }
    }
    else if (m_inStream || m_outStream)
    {
        return IN_PROGRESS;
    }
    return FAILED;
}

/// FIXME We should pass in the Bridge here, not at construction time.
Atlas::Codec * StreamConnect::getCodec(Atlas::Bridge & bridge)
{
    if (m_canPacked) { return new Atlas::Codecs::Packed(m_inStream, m_outStream, bridge); }
    if (m_canXML) { return new Atlas::Codecs::XML(m_inStream, m_outStream, bridge); }
    if (m_canBach) { return new Atlas::Codecs::Bach(m_inStream, m_outStream, bridge); }
    return nullptr;
}

void StreamConnect::processServerCodecs()
{
    std::list<std::string>::iterator j;
    
    for (j = m_inFilters.begin(); j != m_inFilters.end(); ++j)
    {
        if (*j == "XML") { m_canXML = true; }
        if (*j == "Packed") { m_canPacked = true; }
        if (*j == "Bach") { m_canBach = true; }
    }
}
  
void StreamConnect::processServerFilters()
{
    std::list<std::string>::iterator j;
    
    for (j = m_inFilters.begin(); j != m_inFilters.end(); ++j)
    {
        if (*j == "Gzip") { m_canGzip = true; }
        if (*j == "Bzip2") { m_canBzip2 = true; }
    }
}

#if 0
void StreamConnect::processClientCodecs()
{
    std::list<std::string>::const_iterator j;

    for (j = m_inCodecs.begin(); j != m_inCodecs.end(); j++)
    {
        // Do what?
    }
}
  
void StreamConnect::processClientFilters()
{
    std::list<std::string>::const_iterator j;

    for (j = m_inFilters.begin(); j != m_inFilters.end(); j++)
    {
        // Do what?
    }
}
#endif


StreamAccept::StreamAccept(const std::string& name, std::istream& inStream, std::ostream& outStream) :
  m_state(SERVER_GREETING), m_outName(name), m_inStream(inStream), m_outStream(outStream),
  m_codecHelper(m_inCodecs), m_filterHelper(m_inFilters),
  m_canPacked(false), m_canXML(false), m_canBach(false), m_canGzip(false), m_canBzip2(false)
{
}

void StreamAccept::poll(bool can_read)
{
    Debug( std::cout << "** Server(" << m_state << ") : " << std::endl; );

    if (m_state == SERVER_GREETING) 
    {
        // send server greeting

        m_outStream << "ATLAS " << m_outName << std::endl;
        m_state = CLIENT_GREETING;
        Debug( std::cout << "server now in state " << m_state << std::endl; );
    }

    if (can_read) {
        // Cause the stream to read from its socket
        m_inStream.peek();
    }

    std::streamsize count;
    while ((count = m_inStream.rdbuf()->in_avail()) > 0) {
        for (int i = 0 ; i < count; ++i) {
            m_buf += (char) m_inStream.rdbuf()->sbumpc();
        }
    }

    if (m_state == CLIENT_GREETING)
    {
        // get client greeting            
        if (m_buf.size() > 0 && get_line(m_buf, '\n', m_inName) != "")
        {
            Debug(std::cout << "client: " << m_inName << std::endl; );
            m_state = CLIENT_CODECS;
        }
    }

    if (m_state == CLIENT_CODECS)
    {            
        if (m_codecHelper.get(m_buf, "ICAN"))
        {
            m_state = SERVER_CODECS;
            Debug(std::cout << "server now in state " << m_state << std::endl;);
        }
        processClientCodecs();
    }

    if (m_state == SERVER_CODECS)
    {        
        if (m_canPacked) { m_outStream << "IWILL Packed\n"; }
        else if (m_canXML) { m_outStream << "IWILL XML\n"; }
        else if (m_canBach) { m_outStream << "IWILL Bach\n"; }
        m_outStream << std::endl;
        
        m_state = DONE;
    }

#if 0
    if(m_state == CLIENT_FILTERS)
    {
        if (m_filterHelper.get(m_buf, "ICAN"))
        {
            m_state++;
        }
        processClientFilters();
    }

    if (m_state == SERVER_FILTERS)
    {
        //No Filters until they actually work.
        //if (m_canGzip) { m_socket << "IWILL Gzip\n"; }
        //else if (m_canBzip2) { m_socket << "IWILL Bzip2\n"; }
        m_outStream << std::endl;
        m_state++;
    }
#endif
}

Atlas::Negotiate::State StreamAccept::getState()
{
    if (m_state == DONE)
    {    
        if (m_canPacked || m_canXML || m_canBach)
        {
            return SUCCEEDED;
        }
        
        std::cout << "done, but no codec" << std::endl;
    }
    else if (m_inStream || m_outStream)
    {
        return IN_PROGRESS;
    }
    return FAILED;
}

/// FIXME We should pass in the Bridge here, not at construction time.
Atlas::Codec * StreamAccept::getCodec(Atlas::Bridge & bridge)
{
      // XXX XXX XXX XXX
      // should pass an appropriate filterbuf here instead of socket,
      // if we found a filter of course.
      // this poses the problem of the filter being passed by
      // reference, so we'd have to allocate on the heap, but then who
      // would deallocate? erk. -- sdt 2001-01-05
        //return (*outCodecs.begin())-> 
                //New(Codec<std::iostream>::Parameters(m_socket,bridge));
    if (m_canPacked) { return new Atlas::Codecs::Packed(m_inStream, m_outStream, bridge); }
    if (m_canXML) { return new Atlas::Codecs::XML(m_inStream, m_outStream, bridge); }
    if (m_canBach) { return new Atlas::Codecs::Bach(m_inStream, m_outStream, bridge); }
    return nullptr;
}

#if 0
void StreamAccept::processServerCodecs()
{
    FactoryCodecs::iterator i;
    list<std::string>::iterator j;

    FactoryCodecs *myCodecs = Factory<Codec<std::iostream> >::factories();

    for (i = myCodecs->begin(); i != myCodecs->end(); ++i)
    {
        for (j = m_inCodecs.begin(); j != m_inCodecs.end(); ++j)
        {
            if ((*i)->getName() == *j)
            {
                outCodecs.push_back(*i);
                return;              
            }
        }
    }
}
  
void StreamAccept::processServerFilters()
{
  FactoryFilters::iterator i;
    list<std::string>::iterator j;
    
    FactoryFilters *myFilters = Factory<Filter>::factories();

    for (i = myFilters->begin(); i != myFilters->end(); ++i)
    {
        for (j = m_inFilters.begin(); j != m_inFilters.end(); ++j)
        {
            if ((*i)->getName() == *j)
            {
                outFilters.push_back(*i);
            }
        }
    }
}
#endif

void StreamAccept::processClientCodecs()
{
    std::list<std::string>::const_iterator j;
    
    for (j = m_inCodecs.begin(); j != m_inCodecs.end(); j++)
    {    
        if (*j == "XML") { m_canXML = true; }
        if (*j == "Packed") { m_canPacked = true; }
        if (*j == "Bach") { m_canBach = true; }
    }
}
  
void StreamAccept::processClientFilters()
{
    std::list<std::string>::const_iterator j;

    for (j = m_inFilters.begin(); j != m_inFilters.end(); j++)
    {
        if (*j == "Gzip") { m_canGzip = true; }
        if (*j == "Bzip2") { m_canBzip2 = true; }
    }
}

} } // namespace Atlas Net
