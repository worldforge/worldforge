// This file may be redistributed and modified under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day
// Written by Dmitry Derevyanko (dmitryd@linuxfreak.com)

#ifndef ATLAS_STREAM_NEGOTIATE_H
#define ATLAS_STREAM_NEGOTIATE_H

#include "../Net/Socket.h"
#include "Factory.h"
#include "Codec.h"
#include "Filter.h"

#include <string>
#include <vector>
#include <list>

using std::string;
using std::vector;
using std::list;


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


namespace Atlas { namespace Stream {

class FactoryName {
public:

  string name;

  FactoryName(string n) : name(n) { }

  string &GetName() { return name; }

};


typedef Factory<Codec> FactoryCodec;
typedef Factory<Filter> FactoryFilter;

typedef list<FactoryCodec*> FactoryCodecs;
typedef list<FactoryFilter*> FactoryFilters;

typedef list<FactoryName> FactoryNames;

template <class F>
class FactoryHelper {
public:

  typedef list<F*> Fs;

  FactoryNames *in_factories;
  Fs *out_factories;

  FactoryHelper(FactoryNames *i_f, Fs *o_f) :
    in_factories(i_f), out_factories(o_f)
  { }

  bool get(string &buf, string header)
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
	    in_factories->push_back(FactoryName(s));
	    
	    cout << " got: " << s << endl;
	  }
	else
	  cerr << "Unknown pattern " << h << endl;
      }
    return false;
  }

  void put(string &buf, string header)
  {
    Fs::iterator i;

    buf.erase();
    for(i = out_factories->begin(); i != out_factories->end(); i++)
      {
	buf += header;
	buf += " ";
	buf += (*i)->GetName();
	buf += "\n";
      }
    buf += "\n";
  }

};



class Negotiate {
public:

  int state;
  string out_name;
  string in_name;
  Socket *sock;
  FactoryCodecs *my_codecs;
  FactoryNames in_codecs;
  FactoryCodecs out_codecs;
  FactoryFilters *my_filters;
  FactoryNames in_filters;
  FactoryFilters out_filters;
  FactoryHelper<FactoryCodec> codecHelper;
  FactoryHelper<FactoryFilter> filterHelper;
  string buf;

  Negotiate(string &name, Socket *s, FactoryCodecs *fc, FactoryFilters *ff) :
    state(0), out_name(name), sock(s),
    my_codecs(fc), my_filters(ff),
    codecHelper(&in_codecs, &out_codecs),
    filterHelper(&in_filters, &out_filters)
  {
  }

  bool Done() { return state == 10; }
  
  void NegotiateServer()
  {
    cout << "** Server(" << state << ") : " << endl;

    string in;
    string out;

    sock->recv(in);
    buf += in;

    if(state == 0) 
      {
	// send server greeting

	sock->send(out_name);
	sock->send(string("\n"));
	
	state++;
	return;
      }
    if(state == 1)
      {
	// get client greeting
	
	if(buf.size() <= 0
	   || get_line(buf, '\n', in_name) == "")
	  return;

	cout << "client: " << in_name << endl;
	
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

  void processServerCodecs()
  {
    FactoryCodecs::iterator i;
    FactoryNames::iterator j;
    
    for(i = my_codecs->begin(); i != my_codecs->end(); i++)
      for(j = in_codecs.begin(); j != in_codecs.end(); j++)
	if((*i)->GetName() == (*j).GetName())
	  {
	    out_codecs.push_back(*i);
	    return;	      
	  }
  }
  
  void processServerFilters()
  {
    FactoryFilters::iterator i;
    FactoryNames::iterator j;

    for(i = my_filters->begin(); i != my_filters->end(); i++)
      for(j = in_filters.begin(); j != in_filters.end(); j++)
	if((*i)->GetName() == (*j).GetName())
	  {
	    out_filters.push_back(*i);
	  }
  }

  void NegotiateClient()
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
	   || get_line(buf, '\n', in_name) == "")
	  return;

	cout << "server: " << in_name << endl;
	
	state++;
      }
    if(state == 1)
      {
	// send client greeting
	
	sock->send(out_name);
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

  void processClientCodecs()
  {
    out_codecs = *my_codecs;
  }
  
  void processClientFilters()
  {
    out_filters = *my_filters;
  }

};
 
}} // Atlas::Stream

#endif // ATLAS_STREAM_NEGOTIATE_H

