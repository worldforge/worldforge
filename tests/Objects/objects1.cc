#include <iostream>
#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Look.h>

using namespace Atlas;
using namespace Atlas::Objects;
using namespace std;

void list_parents(const Root* r)
{
    typedef Message::Object::ListType::const_iterator Iter;
    
    cout << r->GetAttr("id").AsString() << " parents:" << endl;
    for (Iter I = r->GetAttr("parent").AsList().begin();
            I != r->GetAttr("parent").AsList().end(); I++)
        cout << (*I).AsString() << endl;
}

int main(int argc, char** argv)
{
    Root root;
    Root root_inst = Root::Instantiate();
    root_inst.SetAttr("id", string("root_instantiation"));
    list_parents(&root); list_parents(&root_inst);

    Operation::Look look;
    Operation::Look look_inst = Operation::Look::Instantiate();
    look_inst.SetAttr("id", string("look_instantiation"));
    list_parents(&look); list_parents(&look_inst);
}
