# WFMath

[![Join us on Gitter!](https://badges.gitter.im/Worldforge.svg)](https://gitter.im/Worldforge/Lobby)
[![Appveyor build status](https://ci.appveyor.com/api/projects/status/github/worldforge/wfmath?branch=master&svg=true)](https://ci.appveyor.com/project/erikogenvik/wfmath)
[![Travis build Status](https://travis-ci.com/worldforge/wfmath.svg?branch=master)](https://travis-ci.com/worldforge/wfmath)

This is the [WorldForge](http://worldforge.org/ "The main Worldforge site") math library.
It's licensed under the GPL (see file COPYING).

The primary focus of WFMath is geometric objects. Thus,
it includes several shapes (boxes, balls, lines), in addition
to the basic math objects that are used to build these shapes
(points, vectors, matricies).

## Installation

If you intend to build this as a prerequisite for the Ember client or the Cyphesis server we strongly suggest that you 
use the [Hammer](http://wiki.worldforge.org/wiki/Hammer_Script "The Hammer script") tool to compile it.
This is script provided by the Worldforge project which will download and install all of the required libraries and 
components used by Worldforge.

Alternatively you can use [Conan](https://www.conan.io) to install all dependencies. 
```bash
conan remote add worldforge https://api.bintray.com/conan/worldforge/worldforge-conan
mkdir build && cd build
conan install ../tools/conan --build missing
cmake ..
make -j all install
```

Otherwise the library can most easily be built through the following commands.
```bash
mkdir build && cd build
cmake ..
make -j all install
```

### Tests

The test suite can be built and run using the ```check``` target. For example:

```bash
make check
```

### API documentation

If Doxygen is available API documentation can be generated using the ```dox``` target. For example:

```bash
make dox
```

## Dependencies

The only library that WFMath checks for is Atlas, and it only does this
for the sake of some test functions. The only dependency on Atlas is
in the file atlasconv.h. This header file contains
some inline functions which can be used to convert the various
objects in WFMath to Atlas::Message::Object and back. This
header file will be installed whether Atlas is present or not,
but to use it you must of course have the Atlas headers installed.

## Design

Most of the library classes can be divided into two sorts. The
first kind are basic mathematical objects, whose members are all fundamental
types. The second kind are shapes, which implement the shape class
interface described in doc/shape.h. There are four classes of the first kind:

* Vector<>	A basic mathematical vector
* RotMatrix<>	An orthogonal matrix of determinant 1, useful for
		describing rotations.
* Point<>		A point in space. This basic class also implements
		the shape interface in doc/shape.h.
* Quaternion	A quaternion

The shape classes are:

* AxisBox<>	A box oriented parallel to the coordinate axes
* Ball<>		Ball<2> is a circle, Ball<3> is a sphere, etc.
* Segment<>	A line segment, defined by its endpoints
* RotBox<>	Like AxisBox<>, but it can be rotated to arbitrary
		angles
* Polygon<>	A 2 dimensional polygon contained in a (possibly)
		larger dimensional space

The library also contains some probability-related functions,
as well as wrappers for system time and random number functions.


# Contributing 

Anyone interested in contributing to this project shouldl ook at the file doc/CLASS_LAYOUT, which specifies which member
functions all classes should have, and what order they should appear in.
This only applies to significant classes, not things like
ColinearVectors in error.h, which is only there to be thrown as
as exception.

The documentation is at this point confined to the doc/CLASS_LAYOUT file
and comments in the code. There's also a sample shape class definition
in the file shape.h in the doc directory. This has comments about
what member functions all shape classes are supposed to have, and what they
do.

## How to help

If you're interested in helping out with development you should check out these resources:

* [The main Worldforge site](http://worldforge.org/ "The main Worldforge site")
* [Bugs and feature planning on Launchpad](https://launchpad.net/wfmath "WFMath Launchpad entry")
* [Gitter conversation](https://gitter.im/Worldforge/Lobby "Gitter conversation")
* [Mailing lists](http://mail.worldforge.org/lists/listinfo/ "Mailing lists")
