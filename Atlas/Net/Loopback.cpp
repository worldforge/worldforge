// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#include "Loopback.h"

using Atlas::Bridge;

class LoopBridge : public Bridge
{
    public:

    LoopBridge(Bridge* bridge) : m_bridge(bridge) { }
    
    virtual void streamBegin()
    {
	m_bridge->streamBegin();
    }
    
    virtual void streamMessage(const Map&)
    {
	m_bridge->streamMessage(m_mapBegin);
    }
    
    virtual void streamEnd()
    {
	m_bridge->streamEnd();
    }

    virtual void mapItem(const std::string& name, const Map&)
    {
	m_bridge->mapItem(name, m_mapBegin);
    }
    
    virtual void mapItem(const std::string& name, const List&)
    {
	m_bridge->mapItem(name, m_listBegin);
    }
    
    virtual void mapItem(const std::string& name, long data)
    {
	m_bridge->mapItem(name, data);
    }
    
    virtual void mapItem(const std::string& name, double data)
    {
	m_bridge->mapItem(name, data);
    }

    virtual void mapItem(const std::string& name, const std::string& data)
    {
	m_bridge->mapItem(name, data);
    }
    
    virtual void mapEnd()
    {
	m_bridge->mapEnd();
    }
    
    virtual void listItem(const Map&)
    {
	m_bridge->listItem(m_mapBegin);
    }
    
    virtual void listItem(const List&)
    {
	m_bridge->listItem(m_listBegin);
    }
    
    virtual void listItem(long data)
    {
	m_bridge->listItem(data);
    }
    
    virtual void listItem(double data)
    {
	m_bridge->listItem(data);
    }
    
    virtual void listItem(const std::string& data)
    {
	m_bridge->listItem(data);
    }
    
    virtual void listEnd()
    {
	m_bridge->listEnd();
    }

    private:

    Bridge* m_bridge;
};

void Atlas::Net::loopback(Bridge* d1, Bridge* d2, Bridge*& e1, Bridge*& e2)
{
    e1 = new LoopBridge(d1);
    e2 = new LoopBridge(d2);
}
