# Atlas-C++

[![Join us on Gitter!](https://badges.gitter.im/Worldforge.svg)](https://gitter.im/Worldforge/Lobby)
[![Appveyor build status](https://ci.appveyor.com/api/projects/status/github/worldforge/atlas-cpp?branch=master&svg=true)](https://ci.appveyor.com/project/erikogenvik/atlas-cpp)
[![Travis build Status](https://travis-ci.com/worldforge/atlas-cpp.svg?branch=master)](https://travis-ci.com/worldforge/atlas-cpp)
[![Github build status](https://github.com/worldforge/atlas-cpp/actions/workflows/cmake.yml/badge.svg)](https://github.com/worldforge/atlas-cpp/actions/workflows/cmake.yml)

Welcome to Atlas-C++, the C++ reference implementation of the Atlas protocol. The Atlas protocol is the primary protocol
by which servers and clients in the [WorldForge](http://worldforge.org/ "The main Worldforge site") system communicate.

## Documentation

To generate the documentation, simply configure the tree then execute the "docs" target (for example ```make docs```).
This will use doxygen to generate the documentation, which will then reside in doc/.

Please read the file HACKING.md carefully before working on Atlas-C++.

## The specification

The actual Atlas specification can be found under ```protocol/spec``` as a series of filed ending in the ".def" suffix.
These files represent the canonical Atlas definition.

In order to generate the C++ code these files are translated into a ```protocol/spec/xml/atlas.xml``` file through the
use of the ```protocol/spec/tools/def2xml.py``` file, which requires Python. Normally this would be performed by the
CMake build script.

This file (```protocol/spec/xml/atlas.xml```) in turn gets translated into C++ files through the use of
the ```Atlas/Objects/gen_cpp.py``` Python script (which requires Python).This would also normally be performed by the
CMake build script.

Thus, in order to change the protocol one would perform these steps:

* Update any of the files in ```protocol/spec```
* Rebuild the project (through the builder used with CMake)

You shouldn't touch the ```protocol/spec/xml/atlas.xml``` file, nor any of the C++ files generated (these are identified
by having "Automatically generated using gen_cpp.py." added to the top of them).

There are some areas of improvements here. Chief amongst them would be

* Don't use two different formats: either standardize on only using "atlas.xml", or standardize on only using the .def
  files

## Licensing

Atlas-C++ is licensed under the GNU Lesser General Public License. Please see the file COPYING for details as to its
distribution.

Atlas-C++ is provided AS IS with ABSOLUTELY NO WARRANTY. Please see the file COPYING for details.

## Contact

Please see the AUTHORS file for contact information.

## How to help

If you're interested in helping out with development you should check out these resources:

* [The main Worldforge site](http://worldforge.org/ "The main Worldforge site")
* [Bugs and feature planning on Launchpad](https://launchpad.net/atlas-cpp "Atlas-C++ Launchpad entry")
* [Gitter conversation](https://gitter.im/Worldforge/Lobby "Gitter conversation")
* [Mailing lists](http://mail.worldforge.org/lists/listinfo/ "Mailing lists")
