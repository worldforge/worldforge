#ifndef ERIS_PROPERTY_H
#define ERIS_PROPERTY_H

#include <sigc++/object.h>
#include <Atlas/Message/Element.h>

namespace Eris
{
   
typedef SigC::Signal1<void, Atlas::Message::Element&> Getter;    
typedef SigC::Signal1<void, const Atlas::Message::Element&> Setter;

class Property : virtual public SigC::Object
{
public:    
    Getter Get;	///< emitted before the proeprty value is retrieved
    Setter Set;	///< emitted after the property value is set

    const Atlas::Message::Element& getValue()
    {
	Get.emit(_value);
	return _value;
    }

    void setValue(const Atlas::Message::Element& v)
    {
	_value = v;
	Set.emit(v);
    }
    
protected:
    Atlas::Message::Element _value;
};
    
}

#endif
