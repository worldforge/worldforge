// This file may be redistributed and modified under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day, Dmitry Derevyanko

#include "../Stream/Negotiate.h"

using namespace Atlas::Stream;
using namespace Atlas::Net;

class StringSocket  : public Socket {
public:
  
  std::string &rbuf;
  std::string &sbuf;

  StringSocket(std::string &in, std::string &out) : 
    rbuf(in), sbuf(out)
  {
  }
  ~StringSocket(void) { }
  
  int Send(const std::string& data)
  {
    sbuf += data;
    return 1;
  }
  
  int Receive(std::string& buf)
  {
    if(rbuf.empty()) return 0;

    int n = rbuf.size();
    
    buf = rbuf;
    rbuf.erase();

    return n;
  }
  
  void Close() { } 
  
};

string server_buf, client_buf;

Socket *server_sock = new StringSocket(server_buf, client_buf);
Socket *client_sock = new StringSocket(client_buf, server_buf);

string s_name("SERVER BOB");
string c_name("CLIENT 1.0");

int main(void)
{
  Negotiate s(s_name, server_sock);
  Negotiate c(c_name, client_sock);

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
}

