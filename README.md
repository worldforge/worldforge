# Worldforge

[![Join us on Gitter!](https://badges.gitter.im/Worldforge.svg)](https://gitter.im/Worldforge/Lobby)
[![Appveyor build status](https://ci.appveyor.com/api/projects/status/github/worldforge/worldforge?branch=master&svg=true)](https://ci.appveyor.com/project/erikogenvik/worldforge)
[![Build with CMake](https://github.com/worldforge/worldforge/actions/workflows/build-all.yml/badge.svg)](https://github.com/worldforge/worldforge/actions/workflows/build-all.yml)

[![Get it from the Snap Store](https://snapcraft.io/static/images/badges/en/snap-store-black.svg)](https://snapcraft.io/ember)

The [WorldForge](http://worldforge.org/ "The main Worldforge site") project lets you create virtual world system.
We provide servers, clients, tools, protocol and media. You provide the world.

This is the main monorepo which contains all the components needed for you to either host or access a Worldforge virtual
world.

## Installation

The simplest way to install all required dependencies is by using [Conan](https://www.conan.io).

```bash
conan remote add worldforge https://artifactory.ogenvik.org/artifactory/api/conan/conan
conan install . --build missing -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True
cmake --preset conan-release -DCMAKE_INSTALL_PREFIX=./build/install/release
cmake --build --preset conan-release -j --target all
cmake --build --preset conan-release -j --target install
cmake --build --preset conan-release -j --target mediarepo-checkout 
cmake --build --preset conan-release -j --target media-process-install 
```

The last two invocations are only needed if you also want to host your own server.

NOTE: The invocation of the target "media-process-install" is optional. It will go through the raw Subversion assets and
convert .png to .dds as well as scaling down textures. If you omit this step Cyphesis will instead use the raw
Subversion media. Which you might want if you're developing locally.

### Tests

The test suite can be built and run using the ```check``` target. For example:

```bash
cmake --build --preset conan-release --target check
```

### API documentation

If Doxygen is available API documentation can be generated using the ```dox``` target. For example:

```bash
cmake --build --preset conan-release --target dox
```

### Building libraries

By default, all components will be built and only the server and client will be installed. However, you can force
installation of libs also either by setting the CMake variable "NO_LIBS_INSTALL" to "FALSE", or by invoking CMake in the
directory of the lib you want to build.

## Dependencies

We use Conan for our dependency handling. If you're developing locally you can issue this command to setup both a "
debug" and "release" environment.

```bash
conan install -s build_type=Debug . --build missing -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True --update  && conan install . --build missing -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True --update
```

## License

All code is licensed under GPL v3+, unless otherwise stated.

## How to help

If you're interested in helping out with development you should check out these resources:

* [The main Worldforge site](http://worldforge.org/ "The main Worldforge site")
* [Bugs and feature planning on Launchpad](https://launchpad.net/worldforge "Worldforge Launchpad entry")
* [Gitter conversation](https://gitter.im/Worldforge/Lobby "Gitter conversation")
* [Worldforge wiki](http://wiki.worldforge.org "Worldforge wiki")
