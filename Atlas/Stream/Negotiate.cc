// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day, Dmitry Derevyanko

#include "Negotiate.h"

using namespace std;

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

using namespace Atlas::Stream;

template <class T>
NegotiateHelper<T>::NegotiateHelper(list<string> *names, Factories *out_factories) :
  names(names), outFactories(out_factories)
{ 
}

template <class F>
bool NegotiateHelper<F>::get(string &buf, string header)
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
void NegotiateHelper<T>::put(string &buf, string header)
{
  Factories::iterator i;
  
  buf.erase();
  for(i = outFactories->begin(); i != outFactories->end(); i++)
    {
      buf += header;
      buf += " ";
      buf += (*i)->GetName();
      buf += "\n";
    }
  buf += "\n";
}

Negotiate::Negotiate(const string& name, iostream& s) :
  state(SERVER_GREETING), outName(name), socket(s),
  codecHelper(&inCodecs, &outCodecs),
  filterHelper(&inFilters, &outFilters)
{
}

bool Negotiate::done() 
{
    return (state == DONE);
}

void Negotiate::negotiateServer()
{
    cout << "** Server(" << state << ") : " << endl;

    string out;

    while (socket.rdbuf()->in_avail() || socket.rdbuf()->showmanyc())
    {
	buf += socket.get();
    }

    if (state == SERVER_GREETING) 
    {
	// send server greeting

	socket << outName << '\n';
	state++;
    }
    
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
	socket << out;
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
	socket << out;
	state++;
    }
}

void Negotiate::negotiateClient()
{
    cout << "** Client(" << state << ") : " << endl;

    string out;

    while (socket.rdbuf()->in_avail() || socket.rdbuf()->showmanyc())
    {
	buf += socket.get();
    }

    if(state == SERVER_GREETING)
    {
	// get server greeting

	if(buf.size() <= 0 || get_line(buf, '\n', inName) == "") return;
	cout << "server: " << inName << endl;
	
	state++;
    }

    if(state == CLIENT_GREETING)
    {
	// send client greeting
	
	socket << outName << '\n';
	state++;
    }
    
    if (state == CLIENT_CODECS)
    {
	processClientCodecs();
	codecHelper.put(out, "ICAN");
	socket << out;
	state++;
    }

    if(state == SERVER_CODECS)
    {
	if (codecHelper.get(buf, "IWILL"))
	{
	    state++;
	}
    }
    
    if (state == CLIENT_FILTERS)
    {
	processClientFilters();
	filterHelper.put(out, "ICAN");
	socket << out;
	state++;
    }
    
    if (state == SERVER_FILTERS)
    {
	if (filterHelper.get(buf, "IWILL"))
	{
	    state++;
	}
    }
}

void Negotiate::processServerCodecs()
{
    FactoryCodecs::iterator i;
    list<string>::iterator j;

    FactoryCodecs *myCodecs = &Factory<Codec>::Factories();

    for (i = myCodecs->begin(); i != myCodecs->end(); ++i)
    {
	for (j = inCodecs.begin(); j != inCodecs.end(); ++j)
	{
	    if ((*i)->GetName() == *j)
	    {
		outCodecs.push_back(*i);
		return;	      
	    }
	}
    }
}
  
void Negotiate::processServerFilters()
{
  FactoryFilters::iterator i;
    list<string>::iterator j;
    
    FactoryFilters *myFilters = &Factory<Filter>::Factories();

    for (i = myFilters->begin(); i != myFilters->end(); ++i)
    {
	for (j = inFilters.begin(); j != inFilters.end(); ++j)
	{
	    if ((*i)->GetName() == *j)
	    {
		outFilters.push_back(*i);
	    }
	}
    }
}

void Negotiate::processClientCodecs()
{
    FactoryCodecs *myCodecs = &Factory<Codec>::Factories();
    outCodecs = *myCodecs;
}
  
void Negotiate::processClientFilters()
{
    FactoryFilters *myFilters = &Factory<Filter>::Factories();
    outFilters = *myFilters;
}
