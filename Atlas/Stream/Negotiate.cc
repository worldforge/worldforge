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



Negotiate::Negotiate(string &name, Socket *s, FactoryCodecs *fc, FactoryFilters *ff) :
  state(0), outName(name), sock(s),
  myCodecs(fc), myFilters(ff),
  codecHelper(&inCodecs, &outCodecs),
  filterHelper(&inFilters, &outFilters)
{
}

bool Negotiate::done() 
{
  return state == 10; 
}


void Negotiate::negotiateServer()
{
  cout << "** Server(" << state << ") : " << endl;

  string in;
  string out;

  sock->recv(in);
  buf += in;

  if(state == 0) 
    {
      // send server greeting

      sock->send(outName);
      sock->send(string("\n"));
	
      state++;
      return;
    }
  if(state == 1)
    {
      // get client greeting
	
      if(buf.size() <= 0
	 || get_line(buf, '\n', inName) == "")
	return;

      cout << "client: " << inName << endl;
	
      state++;
    }
  if(state == 2)
    {
      if(codecHelper.get(buf, "ICAN")) state++;
    }
  if(state == 3)
    {
      processServerCodecs();
      codecHelper.put(out, "IWILL");
      sock->send(out);
      state++;
      return;
    }
  if(state == 4)
    {
      if(filterHelper.get(buf, "ICAN")) state++;
    }
  if(state == 5)
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

  if(state == 0)
    {
      // get server greeting

      if(buf.size() <= 0
	 || get_line(buf, '\n', inName) == "")
	return;

      cout << "server: " << inName << endl;
	
      state++;
    }
  if(state == 1)
    {
      // send client greeting
	
      sock->send(outName);
      sock->send(string("\n"));

      processClientCodecs();

      codecHelper.put(out, "ICAN");
      sock->send(out);

      state++;
      return;
    }
  if(state == 2)
    {
      if(codecHelper.get(buf, "IWILL")) state++;
    }
  if(state == 3)
    {
      processClientFilters();
      filterHelper.put(out, "ICAN");
      sock->send(out);
      state++;
      return;
    }
  if(state == 4)
    {
      if(filterHelper.get(buf, "IWILL")) state++;
    }
  if(state == 5)
    state = 10;
}



void Negotiate::processServerCodecs()
{
  FactoryCodecs::iterator i;
  FactoryNames::iterator j;
    
  for(i = myCodecs->begin(); i != myCodecs->end(); i++)
    for(j = inCodecs.begin(); j != inCodecs.end(); j++)
      if((*i)->GetName() == (*j).GetName())
	{
	  outCodecs.push_back(*i);
	  return;	      
	}
}
  
void Negotiate::processServerFilters()
{
  FactoryFilters::iterator i;
  FactoryNames::iterator j;

  for(i = myFilters->begin(); i != myFilters->end(); i++)
    for(j = inFilters.begin(); j != inFilters.end(); j++)
      if((*i)->GetName() == (*j).GetName())
	{
	  outFilters.push_back(*i);
	}
}


void Negotiate::processClientCodecs()
{
  outCodecs = *myCodecs;
}
  
void Negotiate::processClientFilters()
{
  outFilters = *myFilters;
}
