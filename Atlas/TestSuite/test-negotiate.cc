// This file may be redistributed and modified under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day, Dmitry Derevyanko

#include "../Stream/Negotiate.h"

#include <iostream>

using namespace std;
using namespace Atlas::Stream;

class loopbuf : public streambuf
{
    public:

    loopbuf(string &write, string &read) : write(write), read(read) { }

    virtual int overflow(int c)
    {
	write += c;
	return c;
    }
    
    virtual int uflow()
    {
	int c = read[0];
	read.erase(read.begin());
	return c;
    }
    
    virtual int pbackfail(int c)
    {
	string temp;
	temp += c;
	temp += read;
	read = temp;
	return c;
    }

    virtual int showmanyc()
    {
	return read.size();
    }
    
    private:

    string& write;
    string& read;
};

int main()
{
    string server_buffer, client_buffer;
    loopbuf serverbuf(server_buffer, client_buffer);
    loopbuf clientbuf(client_buffer, server_buffer);    
    iostream client_stream(&clientbuf);
    iostream server_stream(&serverbuf);

    Negotiate s("SERVER BOB", server_stream);
    Negotiate c("CLIENT 1.0", client_stream);

  //while(!s.Done() && !c.Done())
  for(int i=0; i < 10; i++)
    {
      //cout << "SB : " << server_buf.size() << " [" << server_buf << "] "
      //   << "CB : " << client_buf.size() << " [" << client_buf << "] " 
      //   <<endl;
      s.negotiateServer();
      c.negotiateClient();
      if(s.done() && c.done()) break;
    }

  if(!s.done()) cout << "Server NOT done" << endl;
  if(!c.done()) cout << "Client NOT done" << endl;

    return 0;
}

