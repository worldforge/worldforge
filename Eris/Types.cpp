#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <cmath> // for atan2
#include <float.h> // for FLT_EPSILON
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
		v = Coord(outer[3].AsFloat(), outer[4].AsFloat(), outer[5].AsFloat());
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

Quaternion::Quaternion() :
    x(0.), y(0.), z(0.), w(1.0)
{    
}

Quaternion::Quaternion(const Atlas::Message::Object &obj)
{
    const AtlasListType &vec = obj.AsList();
    assert(vec.size() == 4);
    x = vec[0].AsFloat();
    y = vec[1].AsFloat();
    z = vec[2].AsFloat();
    w = vec[3].AsFloat();
}

Quaternion::Quaternion(double roll, double pitch, double yaw)
{
    double cr, cp, cy, sr, sp, sy, cpcy, spsy;
    
    cr = cos(roll/2);
    cp = cos(pitch/2);
    cy = cos(yaw/2);

    sr = sin(roll/2);
    sp = sin(pitch/2);
    sy = sin(yaw/2);
    
    cpcy = cp * cy;
    spsy = sp * sy;

    w = cr * cpcy + sr * spsy;
    x = sr * cpcy - cr * spsy;
    y = cr * sp * cy + sr * cp * sy;
    z = cr * cp * sy - sr * sp * cy;
}

void Quaternion::asMatrix(float m[4][4]) const
{
    double wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

    x2 = x + x; y2 = y + y; z2 = z + z;
    xx = x * x2;   xy = x * y2;   xz = x * z2;
    yy = y * y2;   yz = y * z2;   zz = z * z2;
    wx = w * x2;   wy = w * y2;   wz = w * z2;
    
    m[0][0] = 1.0 - (yy + zz);
    m[0][1] = xy - wz;
    m[0][2] = xz + wy;
    m[0][3] = 0.0;
    
    m[1][0] = xy + wz;
    m[1][1] = 1.0 - (xx + zz);
    m[1][2] = yz - wx;
    m[1][3] = 0.0;
    
    m[2][0] = xz - wy;
    m[2][1] = yz + wx;
    m[2][2] = 1.0 - (xx + yy);
    m[2][3] = 0.0;
    
    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = 0;
    m[3][3] = 1;
}

/* this code is stolen from stage/math/matrix3d.cppp by Rakshasa */
Coord Quaternion::asEuler() const
{
    float matrix[4][4];
    asMatrix(matrix);
    
    double x, y, z;
    double cy = sqrt(matrix[0][0] * matrix[0][0] + matrix[1][0] * matrix[1][0]);

    if (cy > 16 * FLT_EPSILON) {
      z = atan2(matrix[2][1], matrix[2][2]);
      y = atan2(-matrix[2][0], cy);
      x = atan2(matrix[1][0], matrix[0][0]);
    } else {
      z = atan2(-matrix[1][2], matrix[1][1]);
      y = atan2(-matrix[2][0], cy);
      x = 0.0;
    }

    return Coord(x,y,z);
}

const Atlas::Message::Object Quaternion::asObject() const
{
    AtlasListType ret;
    ret.push_back(x);
    ret.push_back(y);
    ret.push_back(z);
    ret.push_back(w);
    return ret;
}

};
