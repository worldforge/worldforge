#include <iostream>
#include "../Message/QueuedDecoder.h"
#include "../Message/Layer.h"

using namespace Atlas;
using namespace std;

int main(int argc, char** argv)
{
    Message::QueuedDecoder b;
    Message::Encoder e(&b);

    b.StreamBegin();
    b.StreamMessage(Bridge::MapBegin);
        b.MapItem("integer", 1234);
        b.MapItem("list", Bridge::ListBegin);
            b.ListItem(4321);
            b.ListItem(43.21);
            b.ListItem("4321");
        b.ListEnd();
        b.MapItem("float", 12.34);
        b.MapItem("string", "1234");
    b.MapEnd();
    b.StreamMessage(Bridge::MapBegin);
        b.MapItem("second", "object");
    b.MapEnd();

    Message::Object obj = Message::Object::mkMap();
    obj.set("int", 1234);
    obj.set("float", 12.34);
    obj.set("string", "abcd");

    e.SendMessage(obj);

    b.StreamEnd();

    while (b.QueueSize()) {
    cout << "Queue size: " << b.QueueSize() << endl;
    Message::Object o = b.Pop();
    cout << "Is map: " << o.isMap() << endl;
    cout << "Size: " << o.length() << endl;
    
    Message::Object keys, vals;
    keys = o.keys();
    vals = o.vals();
    for (int i = 0; i < o.length(); i++) {
        Message::Object key, val;
        keys.get(i, key);
        cout << "\"" << key.asString() << "\" = ";
        vals.get(i, val);
        if (val.isInt()) cout << val.asInt();
        if (val.isFloat()) cout << val.asFloat();
        if (val.isString()) cout << "\"" << val.asString() << "\"";
        if (val.isList()) {
            cout << "(";
            for (int j = 0; j < val.length(); j++) {
                Message::Object item;
                val.get(j, item);
                if (item.isInt()) cout << item.asInt();
                if (item.isFloat()) cout << item.asFloat();
                if (item.isString()) cout << "\"" << item.asString() << "\"";
                if (j < val.length() - 1) cout << ", ";
            }
            cout << ")";
        }
        cout << endl;
    }
    cout << "---" << endl;
    }
}
