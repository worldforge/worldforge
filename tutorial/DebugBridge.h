#include <Atlas/Bridge.h>
#include <iostream>

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
        std::cout << padding << "StreamEnd" << endl;
        removePadding();
    }
    
    virtual void MapItem(const std::string& name, const Map&)
    {
        std::cout << name << " -> New Map" << endl;
        addPadding();
    }
    virtual void MapItem(const std::string& name, const List&)
    {
        std::cout << name << " -> New List" << endl;
        addPadding();
    }
    virtual void MapItem(const std::string& name, int i)
    {
        std::cout << name << " -> Int: " << i << endl;
    }
    virtual void MapItem(const std::string& name, double d)
    {
        std::cout << name << " -> Float: " << d << endl;
    }
    virtual void MapItem(const std::string& name, const std::string& s)
    {
        std::cout << name << " -> String: " << s << endl;
    }
    virtual void MapEnd()
    {
        std::cout << "MapEnd" << endl;
        removePadding();
    }
    
    virtual void ListItem(const Map&)
    {
        std::cout << "New Map" << endl;
        addPadding();
    }
    virtual void ListItem(const List&)
    {
        std::cout << "New List" << endl;
        addPadding();
    }
    virtual void ListItem(int i)
    {
        std::cout << "Int: " << i << endl;
    }
    virtual void ListItem(double d)
    {
        std::cout << "Float: " << d << endl;
    }
    virtual void ListItem(const std::string& s)
    {
        std::cout << "String: " << s << endl;
    }
    virtual void ListEnd()
    {
        std::cout << "ListEnd" << endl;
        removePadding();
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
