// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day, Dmitry Derevyanko

#include "Stream.h"

#include <iostream>

#include "../Codecs/XML.h"
#include "../Codecs/Packed.h"
#include "../Codecs/Bach.h"

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

static std::string get_line(std::string &s1, char ch, std::string &s2)
{
  s2 = get_line(s1, ch);

  return s2;
}


Atlas::Net::NegotiateHelper::NegotiateHelper(std::list<std::string> *names) :
  m_names(names)
{ 
}

bool Atlas::Net::NegotiateHelper::get(std::string &buf, const std::string & header)
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
          m_names->push_back(s);
            
          std::cout << " got: " << s << std::endl;
        }
      else
        std::cerr << "Unknown pattern " << h << std::endl;
    }
  return false;
}

void Atlas::Net::NegotiateHelper::put(std::string &buf, const std::string & header)
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

Atlas::Net::StreamConnect::StreamConnect(const std::string& name, std::iostream& s,
Bridge* bridge) :
  m_state(SERVER_GREETING), m_outName(name), m_socket(s), m_bridge(bridge),
  m_codecHelper(&m_inCodecs), m_filterHelper(&m_inFilters),
  m_canPacked(true), m_canXML(true), m_canBach(true),m_canGzip(true), m_canBzip2(true)
{
}

void Atlas::Net::StreamConnect::poll(bool can_read)
{
    std::cout << "** Client(" << m_state << ") : " << std::endl;

    std::string out;

    do
    {
        if (can_read || m_socket.rdbuf()->in_avail()) m_buf += (char) m_socket.get();

    if(m_state == SERVER_GREETING)
    {
        // get server greeting

        if (m_buf.size() > 0 && get_line(m_buf, '\n', m_inName) != "")
        {
            std::cout << "server: " << m_inName << std::endl;
            m_state++;
        }
    }

    if(m_state == CLIENT_GREETING)
    {
        // send client greeting
        
        m_socket << "ATLAS " << m_outName << std::endl;
        m_state++;
    }
    
    if (m_state == CLIENT_CODECS)
    {
        //processClientCodecs();
        m_codecHelper.put(out, "ICAN");
        m_socket << out << std::flush;
        m_state++;
    }

    if(m_state == SERVER_CODECS)
    {
        if (m_codecHelper.get(m_buf, "IWILL"))
        {
            processServerCodecs();
            m_state++;
        }
    }
    
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
    }
    while ((m_state != DONE) && (m_socket.rdbuf()->in_avail()));
}

Atlas::Negotiate<std::iostream>::State Atlas::Net::StreamConnect::getState()
{
    if (m_state == DONE)
    {
        if (m_canPacked || m_canXML || m_canBach)
        {
            return SUCCEEDED;
        }
    }
    else if (m_socket)
    {
        return IN_PROGRESS;
    }
    return FAILED;
}

Atlas::Codec<std::iostream>* Atlas::Net::StreamConnect::getCodec()
{
    if (m_canPacked) { return new Atlas::Codecs::Packed(m_socket, m_bridge); }
    if (m_canXML) { return new Atlas::Codecs::XML(m_socket, m_bridge); }
    if (m_canBach) { return new Atlas::Codecs::Bach(m_socket, m_bridge); }
    return NULL; // throw exception?
}

void Atlas::Net::StreamConnect::processServerCodecs()
{
    std::list<std::string>::iterator j;
    
    for (j = m_inFilters.begin(); j != m_inFilters.end(); ++j)
    {
        if (*j == "XML") { m_canXML = true; }
        if (*j == "Packed") { m_canPacked = true; }
	if (*j == "Bach") { m_canBach = true; }
    }
}
  
void Atlas::Net::StreamConnect::processServerFilters()
{
    std::list<std::string>::iterator j;
    
    for (j = m_inFilters.begin(); j != m_inFilters.end(); ++j)
    {
        if (*j == "Gzip") { m_canGzip = true; }
        if (*j == "Bzip2") { m_canBzip2 = true; }
    }
}

#if 0
void Atlas::Net::StreamConnect::processClientCodecs()
{
    std::list<std::string>::const_iterator j;

    for (j = m_inCodecs.begin(); j != m_inCodecs.end(); j++)
    {
        // Do what?
    }
}
  
void Atlas::Net::StreamConnect::processClientFilters()
{
    std::list<std::string>::const_iterator j;

    for (j = m_inFilters.begin(); j != m_inFilters.end(); j++)
    {
        // Do what?
    }
}
#endif

Atlas::Net::StreamAccept::StreamAccept(const std::string& name, std::iostream& s,
Bridge* bridge) :
  m_state(SERVER_GREETING), m_outName(name), m_socket(s), m_bridge(bridge),
  m_codecHelper(&m_inCodecs), m_filterHelper(&m_inFilters),
  m_canPacked(false), m_canXML(false), m_canGzip(false), m_canBzip2(false)
{
}

void Atlas::Net::StreamAccept::poll(bool can_read)
{
    std::cout << "** Server(" << m_state << ") : " << std::endl;

    std::string out;

    if (m_state == SERVER_GREETING) 
    {
        // send server greeting

        m_socket << "ATLAS " << m_outName << std::endl;
        m_state++;
    }

    do
    {
        if (can_read || m_socket.rdbuf()->in_avail()) m_buf += (char) m_socket.get();

        if (m_state == CLIENT_GREETING)
        {
            // get client greeting
        
            if (m_buf.size() > 0 && get_line(m_buf, '\n', m_inName) != "")
            {
                std::cout << "client: " << m_inName << std::endl;
                m_state++;
            }
        }

        if (m_state == CLIENT_CODECS)
        {
            if (m_codecHelper.get(m_buf, "ICAN"))
            {
                m_state++;
            }
            processClientCodecs();
        }

        if (m_state == SERVER_CODECS)
        {
            if (m_canPacked) { m_socket << "IWILL Packed\n"; }
            else if (m_canXML) { m_socket << "IWILL XML\n"; }
	    else if (m_canBach) { m_socket << "IWILL Bach\n"; }
            m_socket << std::endl;
            m_state++;
        }

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
            m_socket << std::endl;
            m_state++;
        }
    }
    while ((m_state != DONE) && (m_socket.rdbuf()->in_avail()));
}

Atlas::Negotiate<std::iostream>::State Atlas::Net::StreamAccept::getState()
{
    if (m_state == DONE)
    {
        if (m_canPacked || m_canXML || m_canBach)
        {
            return SUCCEEDED;
        }
    }
    else if (m_socket)
    {
        return IN_PROGRESS;
    }
    return FAILED;
}

Atlas::Codec<std::iostream>* Atlas::Net::StreamAccept::getCodec()
{
      // XXX XXX XXX XXX
      // should pass an appropriate filterbuf here instead of socket,
      // if we found a filter of course.
      // this poses the problem of the filter being passed by
      // reference, so we'd have to allocate on the heap, but then who
      // would deallocate? erk. -- sdt 2001-01-05
        //return (*outCodecs.begin())-> 
                //New(Codec<std::iostream>::Parameters(m_socket,bridge));
    if (m_canPacked) { return new Atlas::Codecs::Packed(m_socket, m_bridge); }
    if (m_canXML) { return new Atlas::Codecs::XML(m_socket, m_bridge); }
    if (m_canBach) { return new Atlas::Codecs::Bach(m_socket, m_bridge); }
    return NULL; // throw exception?
}

#if 0
void Atlas::Net::StreamAccept::processServerCodecs()
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
  
void Atlas::Net::StreamAccept::processServerFilters()
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

void Atlas::Net::StreamAccept::processClientCodecs()
{
    std::list<std::string>::const_iterator j;

    for (j = m_inCodecs.begin(); j != m_inCodecs.end(); j++)
    {
        if (*j == "XML") { m_canXML = true; }
        if (*j == "Packed") { m_canPacked = true; }
        if (*j == "Bach") { m_canBach = true; }
    }
}
  
void Atlas::Net::StreamAccept::processClientFilters()
{
    std::list<std::string>::const_iterator j;

    for (j = m_inFilters.begin(); j != m_inFilters.end(); j++)
    {
        if (*j == "Gzip") { m_canGzip = true; }
        if (*j == "Bzip2") { m_canBzip2 = true; }
    }
}
