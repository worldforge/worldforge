#ifndef ERIS_PROPERTY_H
#define ERIS_PROPERTY_H

#include <sigc++/object.h>
#include <Atlas/Message/Object.h>

namespace Eris
{
   
typedef SigC::Signal0<Atlas::Message::Object> Getter;    
typedef SigC::Signal1<void, const Atlas::Message::Object&> Setter;

class Property : public SigC::Object
{
public:    
    Getter get;
    Setter set;

    Atlas::Message::Object value;
};
    
}

#endif
