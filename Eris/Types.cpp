#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include "Types.h"

typedef Atlas::Message::Object::ListType AtlasListType;

namespace Eris
{
	
Coord::Coord(const Atlas::Message::Object &obj)
{
	const AtlasListType &vec = obj.AsList();
	x = vec[0].AsFloat();
	y = vec[1].AsFloat();
	z = vec[2].AsFloat();
}
	
const Atlas::Message::Object Coord::asObject() const
{
	AtlasListType vec;
	
	vec.push_back(x);
	vec.push_back(y);
	vec.push_back(z);
	
	return Atlas::Message::Object(vec);
}

Coord& Coord::operator=(const Atlas::Message::Object &obj)
{
	const AtlasListType &vec = obj.AsList();
	x = vec[0].AsFloat();
	y = vec[1].AsFloat();
	z = vec[2].AsFloat();
	return *this;
}

BBox::BBox(const Atlas::Message::Object &obj)
{
	assert(obj.IsList());
	const AtlasListType &outer(obj.AsList());
	
	if (outer.size() == 1) {
		// one vector
		u = Coord(0., 0., 0.);
		v = Coord(outer[0]);
	} else if (outer.size() == 2) {
		// two vectors
		u = Coord(outer[0]);
		v = Coord(outer[1]);
	} else if (outer.size() == 3) {
		// only maxes, u=[0,0,0]
		u = Coord(0., 0., 0.);
		v = Coord(outer);
	} else {
		assert(outer.size() == 6);
		u = Coord(outer);
		v = Coord(outer[3].AsFloat(), outer[3].AsFloat(), outer[5].AsFloat());
	}
}

BBox::BBox(double ux,
		double uy,
		double uz,
		double vx,
		double vy,
		double vz) :
	u(ux,uy,uz),
	v(vx,vy,vz)
{
}

void BBox::offset(double x, double y, double z)
{
	u.x += x; v.x += x;
	u.y += y; v.y += y;
	u.z += z; v.z += z;
}

};
