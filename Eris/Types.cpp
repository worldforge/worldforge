#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include "Types.h"

namespace Eris
{
	
Coord::Coord(const Atlas::Message::Object &obj)
{
	const Atlas::Message::Object::ListType &vec = obj.AsList();
	x = vec[0].AsFloat();
	y = vec[1].AsFloat();
	z = vec[2].AsFloat();
}
	
Atlas::Message::Object Coord::asObject() const
{
	Atlas::Message::Object::ListType vec;
	
	vec.push_back(x);
	vec.push_back(y);
	vec.push_back(z);
	
	return Atlas::Message::Object(vec);
}

};
