// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day, Dmitry Derevyanko

#include <iostream>

#include "../Codecs/XML.h"
#include "../Codecs/Packed.h"
#include "Stream.h"

std::string get_line(std::string &s, char ch)
{
  std::string out;
  int n = s.find(ch);
  if(n > 0) 
    {
      out.assign(s, 0, n);
      s.erase(0, n+1);
    }

  return out;
}

std::string get_line(std::string &s1, char ch, std::string &s2)
{
  s2 = get_line(s1, ch);

  return s2;
}


Atlas::Net::NegotiateHelper::NegotiateHelper(std::list<std::string> *names) :
  names(names)
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
          names->push_back(s);
            
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
  buf += " Gzip\n";

  buf += header;
  buf += " Bzip2\n";

  buf += "\n";
}

Atlas::Net::StreamConnect::StreamConnect(const std::string& name, std::iostream& s,
Bridge* bridge) :
  state(SERVER_GREETING), outName(name), socket(s), bridge(bridge),
  codecHelper(&inCodecs), filterHelper(&inFilters),
  m_canPacked(true), m_canXML(true), m_canGzip(true), m_canBzip2(true)
{
}

void Atlas::Net::StreamConnect::poll(bool can_read = true)
{
    std::cout << "** Client(" << state << ") : " << std::endl;

    std::string out;

    do
    {
        if (can_read || socket.rdbuf()->in_avail()) buf += socket.get();

    if(state == SERVER_GREETING)
    {
        // get server greeting

        if (buf.size() > 0 && get_line(buf, '\n', inName) != "")
        {
            std::cout << "server: " << inName << std::endl;
            state++;
        }
    }

    if(state == CLIENT_GREETING)
    {
        // send client greeting
        
        socket << "ATLAS " << outName << std::endl;
        state++;
    }
    
    if (state == CLIENT_CODECS)
    {
        //processClientCodecs();
        codecHelper.put(out, "ICAN");
        socket << out << std::flush;
        state++;
    }

    if(state == SERVER_CODECS)
    {
        if (codecHelper.get(buf, "IWILL"))
        {
            processServerCodecs();
            state++;
        }
    }
    
    if (state == CLIENT_FILTERS)
    {
        //processClientFilters();
        filterHelper.put(out, "ICAN");
        socket << out << std::flush;
        state++;
    }
    
    if (state == SERVER_FILTERS)
    {
        if (filterHelper.get(buf, "IWILL"))
        {
            processServerFilters();
            state++;
        }
    }
    }
    while ((state != DONE) && (socket.rdbuf()->in_avail()));
}

Atlas::Negotiate<std::iostream>::State Atlas::Net::StreamConnect::getState()
{
    if (state == DONE)
    {
        if (m_canPacked || m_canXML)
        {
            return SUCCEEDED;
        }
    }
    else if (socket)
    {
        return IN_PROGRESS;
    }
    return FAILED;
}

Atlas::Codec<std::iostream>* Atlas::Net::StreamConnect::getCodec()
{
    if (m_canPacked) { return new Atlas::Codecs::Packed(socket, bridge); }
    if (m_canXML) { return new Atlas::Codecs::XML(socket, bridge); }
    return NULL; // throw exception?
}

void Atlas::Net::StreamConnect::processServerCodecs()
{
    std::list<std::string>::iterator j;
    
    for (j = inFilters.begin(); j != inFilters.end(); ++j)
    {
        if (*j == "XML") { m_canXML = true; }
        if (*j == "Packed") { m_canPacked = true; }
    }
}
  
void Atlas::Net::StreamConnect::processServerFilters()
{
    std::list<std::string>::iterator j;
    
    for (j = inFilters.begin(); j != inFilters.end(); ++j)
    {
        if (*j == "Gzip") { m_canGzip = true; }
        if (*j == "Bzip2") { m_canBzip2 = true; }
    }
}

#if 0
void Atlas::Net::StreamConnect::processClientCodecs()
{
    std::list<std::string>::const_iterator j;

    for (j = inCodecs.begin(); j != inCodecs.end(); j++)
    {
        // Do what?
    }
}
  
void Atlas::Net::StreamConnect::processClientFilters()
{
    std::list<std::string>::const_iterator j;

    for (j = inFilters.begin(); j != inFilters.end(); j++)
    {
        // Do what?
    }
}
#endif

Atlas::Net::StreamAccept::StreamAccept(const std::string& name, std::iostream& s,
Bridge* bridge) :
  state(SERVER_GREETING), outName(name), socket(s), bridge(bridge),
  codecHelper(&inCodecs), filterHelper(&inFilters),
  m_canPacked(false), m_canXML(false), m_canGzip(false), m_canBzip2(false)
{
}

void Atlas::Net::StreamAccept::poll(bool can_read = true)
{
    std::cout << "** Server(" << state << ") : " << std::endl;

    std::string out;

    if (state == SERVER_GREETING) 
    {
        // send server greeting

        socket << "ATLAS " << outName << std::endl;
        state++;
    }

    do
    {
        if (can_read || socket.rdbuf()->in_avail()) buf += socket.get();

        if (state == CLIENT_GREETING)
        {
            // get client greeting
        
            if (buf.size() <= 0 || get_line(buf, '\n', inName) == "") return;
            std::cout << "client: " << inName << std::endl;
            state++;
        }

        if (state == CLIENT_CODECS)
        {
            if (codecHelper.get(buf, "ICAN"))
            {
                state++;
            }
            processClientCodecs();
        }

        if (state == SERVER_CODECS)
        {
            if (m_canPacked) { socket << "IWILL Packed\n"; }
            else if (m_canXML) { socket << "IWILL XML\n"; }
            socket << std::endl;
            state++;
        }

        if(state == CLIENT_FILTERS)
        {
            if (filterHelper.get(buf, "ICAN"))
            {
                state++;
            }
            processClientFilters();
        }

        if (state == SERVER_FILTERS)
        {
            //No Filters until they actually work.
            //if (m_canGzip) { socket << "IWILL Gzip\n"; }
            //else if (m_canBzip2) { socket << "IWILL Bzip2\n"; }
            socket << std::endl;
            state++;
        }
    }
    while ((state != DONE) && (socket.rdbuf()->in_avail()));
}

Atlas::Negotiate<std::iostream>::State Atlas::Net::StreamAccept::getState()
{
    if (state == DONE)
    {
        if (m_canPacked || m_canXML)
        {
            return SUCCEEDED;
        }
    }
    else if (socket)
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
                //New(Codec<std::iostream>::Parameters(socket,bridge));
    if (m_canPacked) { return new Atlas::Codecs::Packed(socket, bridge); }
    if (m_canXML) { return new Atlas::Codecs::XML(socket, bridge); }
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
        for (j = inCodecs.begin(); j != inCodecs.end(); ++j)
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
        for (j = inFilters.begin(); j != inFilters.end(); ++j)
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

    for (j = inCodecs.begin(); j != inCodecs.end(); j++)
    {
        if (*j == "XML") { m_canXML = true; }
        if (*j == "Packed") { m_canPacked = true; }
    }
}
  
void Atlas::Net::StreamAccept::processClientFilters()
{
    std::list<std::string>::const_iterator j;

    for (j = inFilters.begin(); j != inFilters.end(); j++)
    {
        if (*j == "Gzip") { m_canGzip = true; }
        if (*j == "Bzip2") { m_canBzip2 = true; }
    }
}
