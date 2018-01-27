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

    ~DebugBridge() override = default;

    void streamBegin() override {
        std::cout << padding << "streamBegin" << std::endl;
        addPadding();
    }

    void streamMessage() override {
        std::cout << padding << "New Map" << std::endl;
        addPadding();
    }

    void streamEnd() override {
        removePadding();
        std::cout << padding << "streamEnd" << std::endl;
    }
    
    void mapMapItem(std::string name) override
    {
        std::cout << padding << name << " -> New Map" << std::endl;
        addPadding();
    }
    void mapListItem(std::string name) override
    {
        std::cout << padding << name << " -> New List" << std::endl;
        addPadding();
    }
    void mapIntItem(std::string name, long i) override
    {
        std::cout << padding << name << " -> Int: " << i << std::endl;
    }
    void mapFloatItem(std::string name, double d) override
    {
        std::cout << padding << name << " -> Float: " << d << std::endl;
    }
    void mapStringItem(std::string name, std::string s) override
    {
        std::cout << padding << name << " -> String: " << s << std::endl;
    }
    void mapEnd() override
    {
        removePadding();
        std::cout << padding << "mapEnd" << std::endl;
    }
    
    void listMapItem() override
    {
        std::cout << padding << "New Map" << std::endl;
        addPadding();
    }
    void listListItem() override
    {
        std::cout << padding << "New List" << std::endl;
        addPadding();
    }
    void listIntItem(long i) override
    {
        std::cout << padding << "Int: " << i << std::endl;
    }
    void listFloatItem(double d) override
    {
        std::cout << padding << "Float: " << d << std::endl;
    }
    void listStringItem(std::string s) override
    {
        std::cout << padding << "String: " << s << std::endl;
    }
    void listEnd() override
    {
        removePadding();
        std::cout << padding << "listEnd" << std::endl;
    }

protected:
    void addPadding()
    {
        padding += "  ";
    }

    void removePadding()
    {
        padding.erase(padding.end() - 2, padding.end());
    }

    std::string padding;
};
