#ifndef ATLAS_CAST_H
#define ATLAS_CAST_H

#include <stdexcept>
#include <cassert>

#include <Atlas/Message/Element.h>
#include <Atlas/Objects/Root.h>

namespace Atlas {

template <class T>
const T atlas_cast(const Message::Element &data)
{
	T obj = T::Instantiate();
	if (!data.isMap()) {
		assert(false);
		throw std::invalid_argument("Input message object is not a map");
	}
	
	const Message::Element::MapType & mp = data.asMap();
	
	for (Message::Element::MapType::const_iterator A = mp.begin();
		A != mp.end(); ++A)
	{
		obj.setAttr(A->first, A->second);
	}
	
	return obj;
}

template <class T>
const T atlas_cast(const Objects::Root &data)
{
	T obj = T::Instantiate();
	Message::Element::MapType mp = data.asObject().asMap();
	
	for (Message::Element::MapType::iterator A = mp.begin();
		A != mp.end(); ++A)
	{
		obj.setAttr(A->first, A->second);
	}
	
	return obj;
}

}

#endif
