// This file may be redistributed and modified under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day, Dmitry Derevyanko

#include "Negotiate.h"

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
NegotiateHelper<T>::NegotiateHelper(FactoryNames *in_factories, Factories *out_factories) :
  inFactories(in_factories), outFactories(out_factories)
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
	  inFactories->push_back(FactoryName(s));
	    
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

Negotiate::Negotiate(string& name, Net::Socket* s) :
  state(SERVER_GREETING), outName(name), sock(s),
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

  string in;
  string out;

  sock->recv(in);
  buf += in;

  if(state == SERVER_GREETING) 
    {
      // send server greeting

      sock->send(outName);
      sock->send(string("\n"));
	
      state++;
      return;
    }
  if(state == CLIENT_GREETING)
    {
      // get client greeting
	
      if(buf.size() <= 0
	 || get_line(buf, '\n', inName) == "")
	return;

      cout << "client: " << inName << endl;
	
      state++;
    }
  if(state == CLIENT_CODECS)
    {
      if(codecHelper.get(buf, "ICAN")) state++;
    }
  if(state == SERVER_CODECS)
    {
      processServerCodecs();
      codecHelper.put(out, "IWILL");
      sock->send(out);
      state++;
      return;
    }
  if(state == CLIENT_FILTERS)
    {
      if(filterHelper.get(buf, "ICAN")) state++;
    }
  if(state == SERVER_FILTERS)
    {
      processServerFilters();
      filterHelper.put(out, "IWILL");
      sock->send(out);
      state++;
      return;
    }
  if(state == 6)
    state = 10;
}

void Negotiate::negotiateClient()
{
  cout << "** Client(" << state << ") : " << endl;
  string in;
  string out;

  sock->recv(in);
  buf += in;

  if(state == SERVER_GREETING)
    {
      // get server greeting

      if(buf.size() <= 0
	 || get_line(buf, '\n', inName) == "")
	return;

      cout << "server: " << inName << endl;
	
      state++;
    }
  if(state == CLIENT_GREETING)
    {
      // send client greeting
	
      sock->send(outName);
      sock->send(string("\n"));

      state++;
    }
    if (state == CLIENT_CODECS)
    {

      processClientCodecs();

      codecHelper.put(out, "ICAN");
      sock->send(out);

      state++;
      return;
    }
  if(state == SERVER_CODECS)
    {
      if(codecHelper.get(buf, "IWILL")) state++;
    }
  if(state == CLIENT_FILTERS)
    {
      processClientFilters();
      filterHelper.put(out, "ICAN");
      sock->send(out);
      state++;
      return;
    }
  if(state == SERVER_FILTERS)
    {
      if(filterHelper.get(buf, "IWILL")) state++;
    }
}



void Negotiate::processServerCodecs()
{
    FactoryCodecs::iterator i;
    FactoryNames::iterator j;

    FactoryCodecs *myCodecs = &Factory<Codec>::factories;

    for (i = myCodecs->begin(); i != myCodecs->end(); ++i)
    {
	for (j = inCodecs.begin(); j != inCodecs.end(); ++j)
	{
	    if ((*i)->GetName() == (*j).GetName())
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
    FactoryNames::iterator j;
    
    FactoryFilters *myFilters = &Factory<Filter>::factories;

    for (i = myFilters->begin(); i != myFilters->end(); ++i)
    {
	for (j = inFilters.begin(); j != inFilters.end(); ++j)
	{
	    if ((*i)->GetName() == (*j).GetName())
	    {
		outFilters.push_back(*i);
	    }
	}
    }
}

void Negotiate::processClientCodecs()
{
    FactoryCodecs *myCodecs = &Factory<Codec>::factories;
    outCodecs = *myCodecs;
}
  
void Negotiate::processClientFilters()
{
    FactoryFilters *myFilters = &Factory<Filter>::factories;
    outFilters = *myFilters;
}
