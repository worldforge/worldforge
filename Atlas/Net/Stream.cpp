// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day, Dmitry Derevyanko

#include "Stream.h"

using namespace std;
using namespace Atlas;

string get_line(string &s, char ch)
{
  string out;
  int n = s.find(ch);
  if(n > 0) 
    {
      out.assign(s, 0, n);
      s.erase(0, n+1);
    }

  return out;
}

string get_line(string &s1, char ch, string &s2)
{
  s2 = get_line(s1, ch);

  return s2;
}


template <class T>
Atlas::Net::NegotiateHelper<T>::NegotiateHelper(list<string> *names, Factories *out_factories) :
  names(names), outFactories(out_factories)
{ 
}

template <class F>
bool Atlas::Net::NegotiateHelper<F>::get(string &buf, string header)
{
  string s, h;
  
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
	    
	  cout << " got: " << s << endl;
	}
      else
	cerr << "Unknown pattern " << h << endl;
    }
  return false;
}

template <class T>
void Atlas::Net::NegotiateHelper<T>::put(string &buf, string header)
{
  Factories::iterator i;
  
  buf.erase();
  for(i = outFactories->begin(); i != outFactories->end(); i++)
    {
      buf += header;
      buf += " ";
      buf += (*i)->getName();
      buf += "\n";
    }
  buf += "\n";
}

Atlas::Net::StreamConnect::StreamConnect(const string& name, iostream& s,
Bridge* bridge) :
  state(SERVER_GREETING), outName(name), socket(s), bridge(bridge),
  codecHelper(&inCodecs, &outCodecs),
  filterHelper(&inFilters, &outFilters)
{
}

void Atlas::Net::StreamConnect::poll(bool can_read = true)
{
    cout << "** Client(" << state << ") : " << endl;

    string out;

    do
    {
	if (can_read || socket.rdbuf()->in_avail()) buf += socket.get();

    if(state == SERVER_GREETING)
    {
	// get server greeting

	if (buf.size() > 0 && get_line(buf, '\n', inName) != "")
	{
	    cout << "server: " << inName << endl;
	    state++;
	}
    }

    if(state == CLIENT_GREETING)
    {
	// send client greeting
	
	socket << outName << endl;
	state++;
    }
    
    if (state == CLIENT_CODECS)
    {
	processClientCodecs();
	codecHelper.put(out, "ICAN");
	socket << out << flush;
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
	processClientFilters();
	filterHelper.put(out, "ICAN");
	socket << out << flush;
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

Atlas::Negotiate<iostream>::State Atlas::Net::StreamConnect::getState()
{
    if (state == DONE)
    {
        if (! outCodecs.empty ())
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

Atlas::Codec<iostream>* Atlas::Net::StreamConnect::getCodec()
{
    if (! outCodecs.empty ())
    {
        return (*outCodecs.begin())-> \
                New(Codec<iostream>::Parameters(socket,bridge));
    }
    return NULL; // throw exception?
}


void Atlas::Net::StreamConnect::processServerCodecs()
{
    FactoryCodecs::iterator i;
    list<string>::iterator j;

    outCodecs.erase(outCodecs.begin(), outCodecs.end());

    FactoryCodecs *myCodecs = &Factory<Codec<iostream> >::factories();

    for (i = myCodecs->begin(); i != myCodecs->end(); ++i)
    {
	for (j = inCodecs.begin(); j != inCodecs.end(); ++j)
	{
	    if ((*i)->getName() == *j)
	    {
		outCodecs.push_back(*i);
		cerr << *j << " is the one" << endl << flush;
		return;	      
	    }
	}
    }
}
  
void Atlas::Net::StreamConnect::processServerFilters()
{
  FactoryFilters::iterator i;
    list<string>::iterator j;
    
    FactoryFilters *myFilters = &Factory<Filter>::factories();

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

void Atlas::Net::StreamConnect::processClientCodecs()
{
    FactoryCodecs *myCodecs = &Factory<Codec<iostream> >::factories();
    outCodecs = *myCodecs;
}
  
void Atlas::Net::StreamConnect::processClientFilters()
{
    FactoryFilters *myFilters = &Factory<Filter>::factories();
    outFilters = *myFilters;
}

Atlas::Net::StreamAccept::StreamAccept(const string& name, iostream& s,
Bridge* bridge) :
  state(SERVER_GREETING), outName(name), socket(s), bridge(bridge),
  codecHelper(&inCodecs, &outCodecs),
  filterHelper(&inFilters, &outFilters)
{
}

void Atlas::Net::StreamAccept::poll(bool can_read = true)
{
    cout << "** Server(" << state << ") : " << endl;

    string out;

    if (state == SERVER_GREETING) 
    {
	// send server greeting

	socket << outName << endl;
	state++;
    }

    do
    {
	if (can_read || socket.rdbuf()->in_avail()) buf += socket.get();

    if (state == CLIENT_GREETING)
    {
	// get client greeting
	
	if (buf.size() <= 0 || get_line(buf, '\n', inName) == "") return;
	cout << "client: " << inName << endl;
	state++;
    }
    
    if (state == CLIENT_CODECS)
    {
	if (codecHelper.get(buf, "ICAN"))
	{
	    state++;
	}
    }
    
    if (state == SERVER_CODECS)
    {
	processServerCodecs();
	codecHelper.put(out, "IWILL");
	socket << out << flush;
	state++;
    }
    
    if(state == CLIENT_FILTERS)
    {
	if (filterHelper.get(buf, "ICAN"))
	{
	    state++;
	}
    }
    
    if (state == SERVER_FILTERS)
    {
	processServerFilters();
	filterHelper.put(out, "IWILL");
	socket << out << flush;
	state++;
    }
    }
    while ((state != DONE) && (socket.rdbuf()->in_avail()));
}

Atlas::Negotiate<iostream>::State Atlas::Net::StreamAccept::getState()
{
    if (state == DONE)
    {
        if (! outCodecs.empty ())
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

Atlas::Codec<iostream>* Atlas::Net::StreamAccept::getCodec()
{
    if (! outCodecs.empty ())
    {
        return (*outCodecs.begin())-> \
                New(Codec<iostream>::Parameters(socket,bridge));
    }
    return NULL; // throw exception?
}

void Atlas::Net::StreamAccept::processServerCodecs()
{
    FactoryCodecs::iterator i;
    list<string>::iterator j;

    FactoryCodecs *myCodecs = &Factory<Codec<iostream> >::factories();

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
    list<string>::iterator j;
    
    FactoryFilters *myFilters = &Factory<Filter>::factories();

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

void Atlas::Net::StreamAccept::processClientCodecs()
{
    FactoryCodecs *myCodecs = &Factory<Codec<iostream> >::factories();
    outCodecs = *myCodecs;
}
  
void Atlas::Net::StreamAccept::processClientFilters()
{
    FactoryFilters *myFilters = &Factory<Filter>::factories();
    outFilters = *myFilters;
}
