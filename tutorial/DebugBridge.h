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

    virtual void StreamBegin() {
        std::cout << padding << "StreamBegin" << endl;
        addPadding();
    }
    virtual void StreamMessage(const Map&) {
        std::cout << padding << "New Map" << endl;
        addPadding();
    }
    virtual void StreamEnd() {
        removePadding();
        std::cout << padding << "StreamEnd" << endl;
    }
    
    virtual void MapItem(const std::string& name, const Map&)
    {
        std::cout << padding << name << " -> New Map" << endl;
        addPadding();
    }
    virtual void MapItem(const std::string& name, const List&)
    {
        std::cout << padding << name << " -> New List" << endl;
        addPadding();
    }
    virtual void MapItem(const std::string& name, int i)
    {
        std::cout << padding << name << " -> Int: " << i << endl;
    }
    virtual void MapItem(const std::string& name, double d)
    {
        std::cout << padding << name << " -> Float: " << d << endl;
    }
    virtual void MapItem(const std::string& name, const std::string& s)
    {
        std::cout << padding << name << " -> String: " << s << endl;
    }
    virtual void MapEnd()
    {
        removePadding();
        std::cout << padding << "MapEnd" << endl;
    }
    
    virtual void ListItem(const Map&)
    {
        std::cout << padding << "New Map" << endl;
        addPadding();
    }
    virtual void ListItem(const List&)
    {
        std::cout << padding << "New List" << endl;
        addPadding();
    }
    virtual void ListItem(int i)
    {
        std::cout << padding << "Int: " << i << endl;
    }
    virtual void ListItem(double d)
    {
        std::cout << padding << "Float: " << d << endl;
    }
    virtual void ListItem(const std::string& s)
    {
        std::cout << padding << "String: " << s << endl;
    }
    virtual void ListEnd()
    {
        removePadding();
        std::cout << padding << "ListEnd" << endl;
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
