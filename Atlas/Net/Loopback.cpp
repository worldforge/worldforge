// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#include "Loopback.h"

using namespace std;
using namespace Atlas;

class LoopBridge : public Bridge
{
    public:

    LoopBridge(Bridge* bridge) : bridge(bridge) { }
    
    virtual void streamBegin()
    {
	bridge->streamBegin();
    }
    
    virtual void streamMessage(const Map&)
    {
	bridge->streamMessage(mapBegin);
    }
    
    virtual void streamEnd()
    {
	bridge->streamEnd();
    }

    virtual void mapItem(const std::string& name, const Map&)
    {
	bridge->mapItem(name, mapBegin);
    }
    
    virtual void mapItem(const std::string& name, const List&)
    {
	bridge->mapItem(name, listBegin);
    }
    
    virtual void mapItem(const std::string& name, int data)
    {
	bridge->mapItem(name, data);
    }
    
    virtual void mapItem(const std::string& name, double data)
    {
	bridge->mapItem(name, data);
    }

    virtual void mapItem(const std::string& name, const std::string& data)
    {
	bridge->mapItem(name, data);
    }
    
    virtual void mapEnd()
    {
	bridge->mapEnd();
    }
    
    virtual void listItem(const Map&)
    {
	bridge->listItem(mapBegin);
    }
    
    virtual void listItem(const List&)
    {
	bridge->listItem(listBegin);
    }
    
    virtual void listItem(int data)
    {
	bridge->listItem(data);
    }
    
    virtual void listItem(double data)
    {
	bridge->listItem(data);
    }
    
    virtual void listItem(const std::string& data)
    {
	bridge->listItem(data);
    }
    
    virtual void listEnd()
    {
	bridge->listEnd();
    }

    private:

    Bridge* bridge;
};

void Atlas::Net::loopback(Bridge* d1, Bridge* d2, Bridge*& e1, Bridge*& e2)
{
    e1 = new LoopBridge(d1);
    e2 = new LoopBridge(d2);
}
