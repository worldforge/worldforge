#ifndef ERIS_TYPES_H
#define ERIS_TYPES_H

// system headers
#include <vector>
#include <string>
#include <list>
#include <set>
#include <deque>

#include <stdexcept>

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>

#ifndef WFMATH_CONVERSION
#include <wfmath/vector.h>
#include <wfmath/point.h>
#include <wfmath/quaternion.h>
#include <wfmath/axisbox.h>
#include <wfmath/stream.h>
#endif

#define ERIS_HAVE_WFMATH

#include <sigc++/basic_signal.h>

namespace Eris
{

typedef std::list<std::string> StringList;	
typedef std::set<std::string> StringSet;

/** This is the Eris base for all exceptions; note it inherits from std::except,
which isn't ideal. One option would be to refactor the various final
exceptions so they inherit from the 'closest' ISO C++ exception, but it
hardly seems worth it. */
class BaseException : public std::runtime_error
{
public:
	BaseException(const std::string &m) : 
		std::runtime_error(m), _msg(m) {;}
        virtual ~BaseException() throw() { }
	const std::string _msg;
};	
	
class InvalidOperation : public BaseException
{
public:
	InvalidOperation(const std::string &m) : BaseException(m) {;}
        virtual ~InvalidOperation() throw() { }
};
	
/// throw when processing encounters an Atlas message that deviates from the spec

class IllegalMessage : public BaseException
{
public:
	IllegalMessage(const Atlas::Message::Object &m, const std::string &s) :
		BaseException(s), what(m) {;}
        virtual ~IllegalMessage() throw() { }
			
	Atlas::Message::Object what;
};

/// Thrown when processing an Atlas::Object that isn't what we expected
class IllegalObject : public BaseException
{
public:
	IllegalObject(const Atlas::Objects::Root &o, const std::string &s) :
		BaseException(s), what(o) {;}
        virtual ~IllegalObject() throw() { }
			
	Atlas::Objects::Root what;
};

class NetworkFailure : public BaseException
{
public:
	NetworkFailure(const std::string &s) :
		BaseException(s) {;}
        virtual ~NetworkFailure() throw() { }
};


typedef std::list<Atlas::Message::Object> MessageList;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef WFMATH_CONVERSION
class Coord
{
public:
	Coord(const Atlas::Message::Object &obj);
	Coord(double a = 0., double b=0., double c=0.) :
		x(a), y(b), z(c)
	{;}
	Coord(const WFMath::Point<3> &p) : x(p.x()), y(p.y()), z(p.z()) {}
	Coord(const WFMath::Vector<3> &v) : x(v.x()), y(v.y()), z(v.z()) {}
	
	Coord& operator=(const Atlas::Message::Object &obj);
		
	const Atlas::Message::Object asObject() const;

        WFMath::Point<3>  asPoint()  {return WFMath::Point<3>(x, y, z);}
        WFMath::Vector<3> asVector() {return WFMath::Vector<3>(x, y, z);}
		
	double x,y,z;
};
	
class BBox
{
public:
	BBox(double ux = 0., double uy = 0., double uz = 0.,
			double vx=1., double vy=1., double vz=1.);
	BBox(const Atlas::Message::Object &obj);
	BBox(const WFMath::AxisBox<3>& a) : u(a.lowCorner()), v(a.highCorner()) {}
	
	const Atlas::Message::Object asObject() const;

	WFMath::AxisBox<3> asAxisBox()
		{return WFMath::AxisBox<3>(u.asPoint(), v.asPoint());}

	Coord u,v;
};

class Quaternion
{
public:
    Quaternion();

    /** construct from Atlas data */
    Quaternion(const Atlas::Message::Object &obj);
    Quaternion(const WFMath::Quaternion& q) : x(q.vector().x()), y(q.vector().y()),
					      z(q.vector().z()), w(q.scalar()) {}

    /** construct from an Euler triple */
    Quaternion(double rool, double pitch, double yaw);

    const Atlas::Message::Object asObject() const;

    /** convert to a 4x4 rotation matrix in column major format (i.e suitable for
     plugging into OpenGL directly. */
    void asMatrix(float m[4][4]) const;

    /** convert to an euler triple, i.e rotation about X/Y/Z. Note the final result is dependant on applying the rotations
    in the correct order. */
    Coord asEuler() const;

    WFMath::Quaternion asQuaternion() {return WFMath::Quaternion(w, x, y, z);}

    double x,y,z,w;
};
#endif // WFMATH_CONVERSION

// Forward Decls
class Entity;
typedef Entity* EntityPtr;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
	#include <winsock.h>
	typedef SOCKET Socket;
#else
	typedef int Socket;
#endif

// would prefer a set (for faster find() impl), but that assumed an ordering
// operation on Sockets; this is fine for Unix, but less certain for Win32
typedef std::list<Socket> SocketList;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// A Generic 'event' occurred signal, especially for use with Wait exceptions
typedef SigC::Signal0<void> Signal;

/** OperationBlocked exceptions are thrown by certain Eris functions when they cannot
proceed (almost always becuase they are waiting on data from the server).  They
are deliberately placed outside the standard exception heirarchy so people
don't accidently catch them.

The member signal is emitted when the original operation can be restarted
(i.e the blocking condition no longer exists). The major complication is storing sufficent
data to restart the request. */

class OperationBlocked
{
public:
	OperationBlocked(Signal &rsig) : 
		_continue(rsig)
	{;}
	
	Signal& _continue;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}	

#endif
