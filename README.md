# Atlas-C++

[![Join us on Gitter!](https://badges.gitter.im/Worldforge.svg)](https://gitter.im/Worldforge/Lobby)
[![Appveyor build status](https://ci.appveyor.com/api/projects/status/github/worldforge/atlas-cpp?branch=master&svg=true)](https://ci.appveyor.com/project/erikogenvik/atlas-cpp)
[![Travis build Status](https://travis-ci.com/worldforge/atlas-cpp.svg?branch=master)](https://travis-ci.com/worldforge/atlas-cpp)

Welcome to Atlas-C++, the C++ reference implementation of the Atlas protocol. The Atlas protocol is the primary protocol
 by which servers and clients in the [WorldForge](http://worldforge.org/ "The main Worldforge site") system communicate.

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

## The specification

The actual Atlas specification can be found under ```protocol/spec``` as a series of filed ending in the ".def" suffix. These files represent the canonical Atlas definition.

In order to generate the C++ code these files are translated into a ```protocol/spec/xml/atlas.xml``` file through the use of the ```protocol/spec/tools/def2xml.py``` file, which requires Python 2. Normally this would be performed by the CMake build script.

This file (```protocol/spec/xml/atlas.xml```) in turn gets translated into C++ files through the use of the ```Atlas/Objects/gen_cpp.py``` Python script (which requires Python 2).This would also normally be performed by the CMake build script. 

Thus, in order to change the protocol one would perform these steps:
 * Update any of the files in ```protocol/spec```
 * Rebuild the project (through the builder used with CMake)
 
You shouldn't touch the ```protocol/spec/xml/atlas.xml``` file, nor any of the C++ files generated (these are identified by having "Automatically generated using gen_cpp.py." added to the top of them).
 
There are some areas of improvements here. Chief amongst them would be
 * Don't use two different formats: either standardize on only using "atlas.xml", or standardize on only using the .def files
 * Convert all code to Python 3 

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
