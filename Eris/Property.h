#ifndef ERIS_PROPERTY_H
#define ERIS_PROPERTY_H

#include <sigc++/object.h>
#include <Atlas/Message/Object.h>

namespace Eris
{
   
typedef SigC::Signal1<void, Atlas::Message::Object&> Getter;    
typedef SigC::Signal1<void, const Atlas::Message::Object&> Setter;

class Property : public SigC::Object
{
public:    
    Getter Get;	///< emitted before the proeprty value is retrieved
    Setter Set;	///< emitted after the property value is set

    const Atlas::Message::Object& getValue()
    {
	Get.emit(_value);
	return _value;
    }

    void setValue(const Atlas::Message::Object& v)
    {
	_value = v;
	Set.emit(v);
    }
    
protected:
    Atlas::Message::Object _value;
};
    
}

#endif
