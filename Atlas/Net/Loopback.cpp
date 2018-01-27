// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

// $Id$

#include <Atlas/Net/Loopback.h>

namespace Atlas { namespace Net {

class LoopBridge : public Bridge
{
    public:

	explicit LoopBridge(Bridge* bridge) : m_bridge(bridge) { }

    void streamBegin() override
    {
	m_bridge->streamBegin();
    }
    
    void streamMessage() override
    {
	m_bridge->streamMessage();
    }
    
    void streamEnd() override
    {
	m_bridge->streamEnd();
    }

    void mapMapItem(std::string name) override
    {
	m_bridge->mapMapItem(std::move(name));
    }
    
    void mapListItem(std::string name) override
    {
	m_bridge->mapListItem(std::move(name));
    }
    
    void mapIntItem(std::string name, long data) override
    {
	m_bridge->mapIntItem(std::move(name), data);
    }
    
    void mapFloatItem(std::string name, double data) override
    {
	m_bridge->mapFloatItem(std::move(name), data);
    }

    void mapStringItem(std::string name, std::string data) override
    {
	m_bridge->mapStringItem(std::move(name), std::move(data));
    }
    
    void mapEnd() override
    {
	m_bridge->mapEnd();
    }
    
    void listMapItem() override
    {
	m_bridge->listMapItem();
    }
    
    void listListItem() override
    {
	m_bridge->listListItem();
    }
    
    void listIntItem(long data) override
    {
	m_bridge->listIntItem(data);
    }
    
    void listFloatItem(double data) override
    {
	m_bridge->listFloatItem(data);
    }
    
    void listStringItem(std::string data) override
    {
	m_bridge->listStringItem(std::move(data));
    }
    
    void listEnd() override
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
