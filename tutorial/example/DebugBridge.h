#ifndef DEBUG_BRIDGE_H
#define DEBUG_BRIDGE_H

#include <iostream>
#include <string>
#include <Atlas/Bridge.h>

class DebugBridge : public Atlas::Bridge
{
public:

    DebugBridge()
    {
        padding = "";
    }

    virtual ~DebugBridge() { }

    virtual void streamBegin() {
        std::cout << padding << "streamBegin" << endl;
        addPadding();
    }
    virtual void streamMessage() {
        std::cout << padding << "New Map" << endl;
        addPadding();
    }
    virtual void streamEnd() {
        removePadding();
        std::cout << padding << "streamEnd" << endl;
    }
    
    virtual void mapMapItem(const std::string& name)
    {
        std::cout << padding << name << " -> New Map" << endl;
        addPadding();
    }
    virtual void mapListItem(const std::string& name)
    {
        std::cout << padding << name << " -> New List" << endl;
        addPadding();
    }
    virtual void mapIntItem(const std::string& name, long i)
    {
        std::cout << padding << name << " -> Int: " << i << endl;
    }
    virtual void mapFloatItem(const std::string& name, double d)
    {
        std::cout << padding << name << " -> Float: " << d << endl;
    }
    virtual void mapStringItem(const std::string& name, const std::string& s)
    {
        std::cout << padding << name << " -> String: " << s << endl;
    }
    virtual void mapEnd()
    {
        removePadding();
        std::cout << padding << "mapEnd" << endl;
    }
    
    virtual void listMapItem()
    {
        std::cout << padding << "New Map" << endl;
        addPadding();
    }
    virtual void listListItem()
    {
        std::cout << padding << "New List" << endl;
        addPadding();
    }
    virtual void listIntItem(long i)
    {
        std::cout << padding << "Int: " << i << endl;
    }
    virtual void listFloatItem(double d)
    {
        std::cout << padding << "Float: " << d << endl;
    }
    virtual void listStringItem(const std::string& s)
    {
        std::cout << padding << "String: " << s << endl;
    }
    virtual void listEnd()
    {
        removePadding();
        std::cout << padding << "listEnd" << endl;
    }

protected:
    virtual void addPadding()
    {
        padding += "  ";
    }

    virtual void removePadding()
    {
        padding.erase(padding.end() - 2, padding.end());
    }

    std::string padding;
};

#endif // DEBUG_BRIDGE_H
