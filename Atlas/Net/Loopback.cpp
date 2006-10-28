// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

// $Id$

#include <Atlas/Net/Loopback.h>

namespace Atlas { namespace Net {

class LoopBridge : public Bridge
{
    public:

    LoopBridge(Bridge* bridge) : m_bridge(bridge) { }
    
    virtual void streamBegin()
    {
	m_bridge->streamBegin();
    }
    
    virtual void streamMessage()
    {
	m_bridge->streamMessage();
    }
    
    virtual void streamEnd()
    {
	m_bridge->streamEnd();
    }

    virtual void mapMapItem(const std::string& name)
    {
	m_bridge->mapMapItem(name);
    }
    
    virtual void mapListItem(const std::string& name)
    {
	m_bridge->mapListItem(name);
    }
    
    virtual void mapIntItem(const std::string& name, long data)
    {
	m_bridge->mapIntItem(name, data);
    }
    
    virtual void mapFloatItem(const std::string& name, double data)
    {
	m_bridge->mapFloatItem(name, data);
    }

    virtual void mapStringItem(const std::string& name, const std::string& data)
    {
	m_bridge->mapStringItem(name, data);
    }
    
    virtual void mapEnd()
    {
	m_bridge->mapEnd();
    }
    
    virtual void listMapItem()
    {
	m_bridge->listMapItem();
    }
    
    virtual void listListItem()
    {
	m_bridge->listListItem();
    }
    
    virtual void listIntItem(long data)
    {
	m_bridge->listIntItem(data);
    }
    
    virtual void listFloatItem(double data)
    {
	m_bridge->listFloatItem(data);
    }
    
    virtual void listStringItem(const std::string& data)
    {
	m_bridge->listStringItem(data);
    }
    
    virtual void listEnd()
    {
	m_bridge->listEnd();
    }

    private:

    Bridge* m_bridge;
};

void loopback(Bridge* d1, Bridge* d2, Bridge*& e1, Bridge*& e2)
{
    e1 = new LoopBridge(d1);
    e2 = new LoopBridge(d2);
}

} } // namespace Atlas::Net
