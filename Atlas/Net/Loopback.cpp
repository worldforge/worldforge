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
    
    virtual void StreamBegin()
    {
	bridge->StreamBegin();
    }
    
    virtual void StreamMessage(const Map&)
    {
	bridge->StreamMessage(MapBegin);
    }
    
    virtual void StreamEnd()
    {
	bridge->StreamEnd();
    }

    virtual void MapItem(const std::string& name, const Map&)
    {
	bridge->MapItem(name, MapBegin);
    }
    
    virtual void MapItem(const std::string& name, const List&)
    {
	bridge->MapItem(name, ListBegin);
    }
    
    virtual void MapItem(const std::string& name, int data)
    {
	bridge->MapItem(name, data);
    }
    
    virtual void MapItem(const std::string& name, double data)
    {
	bridge->MapItem(name, data);
    }

    virtual void MapItem(const std::string& name, const std::string& data)
    {
	bridge->MapItem(name, data);
    }
    
    virtual void MapEnd()
    {
	bridge->MapEnd();
    }
    
    virtual void ListItem(const Map&)
    {
	bridge->ListItem(MapBegin);
    }
    
    virtual void ListItem(const List&)
    {
	bridge->ListItem(ListBegin);
    }
    
    virtual void ListItem(int data)
    {
	bridge->ListItem(data);
    }
    
    virtual void ListItem(double data)
    {
	bridge->ListItem(data);
    }
    
    virtual void ListItem(const std::string& data)
    {
	bridge->ListItem(data);
    }
    
    virtual void ListEnd()
    {
	bridge->ListEnd();
    }

    private:

    Bridge* bridge;
};

void Atlas::Net::Loopback(Bridge* d1, Bridge* d2, Bridge*& e1, Bridge*& e2)
{
    e1 = new LoopBridge(d1);
    e2 = new LoopBridge(d2);
}
