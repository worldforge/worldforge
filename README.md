# Atlas-C++

[![Join us on Gitter!](https://badges.gitter.im/Worldforge.svg)](https://gitter.im/Worldforge/Lobby)

Welcome to Atlas-C++, the C++ reference implementation of the Atlas protocol. The Atlas protocol is the primary protocol
 by which servers and clients in the [WorldForge](http://worldforge.org/ "The main Worldforge site") system communicate.

## Installation

If you intend to build this as a prerequisite for the Ember client or the Cyphesis server we strongly suggest that you 
use the [Hammer](http://wiki.worldforge.org/wiki/Hammer_Script "The Hammer script") tool to compile Ember.
This is script provided by the Worldforge project which will download and install all of the required libraries and 
components used by Worldforge.

Otherwise the library can most easily be built through the following commands.
```
mkdir build_`arch` && cd build_`arch`
cmake ..
make
make install
```

### Tests

The test suite can be built and run using the ```check``` target. For example:

```
make check
```

### API documentation

If Doxygen is available API documentation can be generated using the ```docs``` target. For example:

```
make docs
```

## Version

This is Atlas-C++ 0.7.x, the latest development version of Atlas-C++. The
0.6.x branch will contain NO API CHANGES. The 0.7.x branch is completely
experimental and may not be easy to track. Any changes affecting the API
should go into 0.7.x, as should any features which are not completely
stable.

## Documentation

To generate the documentation, simply configure the tree then execute the "docs" target (for example ```make docs```). 
This will use doxygen to generate the documentation, which will then reside in doc/.

Please read the file HACKING.md carefully before working on Atlas-C++.

## Licensing

Atlas-C++ is licensed under the GNU Lesser General Public License. Please see
the file COPYING for details as to its distribution.

Atlas-C++ is provided AS IS with ABSOLUTELY NO WARRANTY. Please see the file
COPYING for details.

## Contact

Please see the AUTHORS file for contact information.

## How to help

If you're interested in helping out with development you should check out these resources:

* [The main Worldforge site](http://worldforge.org/ "The main Worldforge site")
* [Bugs and feature planning on Launchpad](https://launchpad.net/atlas-cpp "Atlas-C++ Launchpad entry")
* [Gitter conversation](https://gitter.im/Worldforge/Lobby "Gitter conversation")
* [Mailing lists](http://mail.worldforge.org/lists/listinfo/ "Mailing lists")
