#include <Atlas/Bridge.h>

#include <iostream>
#include <string>

class DebugBridge : public Atlas::Bridge
{
public:

    DebugBridge()
    {
        padding = "";
    }

    virtual ~DebugBridge() { }

    virtual void streamBegin() {
        std::cout << padding << "streamBegin" << std::endl;
        addPadding();
    }
    virtual void streamMessage() {
        std::cout << padding << "New Map" << std::endl;
        addPadding();
    }
    virtual void streamEnd() {
        removePadding();
        std::cout << padding << "streamEnd" << std::endl;
    }
    
    virtual void mapMapItem(const std::string& name)
    {
        std::cout << padding << name << " -> New Map" << std::endl;
        addPadding();
    }
    virtual void mapListItem(const std::string& name)
    {
        std::cout << padding << name << " -> New List" << std::endl;
        addPadding();
    }
    virtual void mapIntItem(const std::string& name, long i)
    {
        std::cout << padding << name << " -> Int: " << i << std::endl;
    }
    virtual void mapFloatItem(const std::string& name, double d)
    {
        std::cout << padding << name << " -> Float: " << d << std::endl;
    }
    virtual void mapStringItem(const std::string& name, const std::string& s)
    {
        std::cout << padding << name << " -> String: " << s << std::endl;
    }
    virtual void mapEnd()
    {
        removePadding();
        std::cout << padding << "mapEnd" << std::endl;
    }
    
    virtual void listMapItem()
    {
        std::cout << padding << "New Map" << std::endl;
        addPadding();
    }
    virtual void listListItem()
    {
        std::cout << padding << "New List" << std::endl;
        addPadding();
    }
    virtual void listIntItem(long i)
    {
        std::cout << padding << "Int: " << i << std::endl;
    }
    virtual void listFloatItem(double d)
    {
        std::cout << padding << "Float: " << d << std::endl;
    }
    virtual void listStringItem(const std::string& s)
    {
        std::cout << padding << "String: " << s << std::endl;
    }
    virtual void listEnd()
    {
        removePadding();
        std::cout << padding << "listEnd" << std::endl;
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
