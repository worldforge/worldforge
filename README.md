# Worldforge

[![Join us on Gitter!](https://badges.gitter.im/Worldforge.svg)](https://gitter.im/Worldforge/Lobby)
[![Appveyor build status](https://ci.appveyor.com/api/projects/status/github/worldforge/worldforge?branch=master&svg=true)](https://ci.appveyor.com/project/erikogenvik/worldforge)
[![Build with CMake](https://github.com/worldforge/worldforge/actions/workflows/build-all.yml/badge.svg)](https://github.com/worldforge/worldforge/actions/workflows/build-all.yml)

[![Get it from the Snap Store](https://snapcraft.io/static/images/badges/en/snap-store-black.svg)](https://snapcraft.io/ember)

The [WorldForge](http://worldforge.org/ "The main Worldforge site") project lets you create virtual world system.
We provide servers, clients, tools, protocol and media. You provide the world. Everything we do is Free Software.

This is the main monorepo which contains all the components needed for you to either host or access a Worldforge virtual
world.

Some of the features we provide.

* Fully scriptable through Python
* Live reload of both rules and world entities; edit your world without having to shut down or reload
* Complete 3d physics simulation
* Complex AI system, using Behavioral Trees and Python scripts
* Out-of-process AI, allowing for distributed AI clients
* Persistence through either SQLite or PostgreSQL
* Powerful built in rules for visibility and containment of entities
* Emergent gameplay through multiple simple systems interacting
* Quick and powerful procedural terrain generation

## Components

These components make up the Worldforge system. Each of them in turn contains a README file which explains more about
how they work.

* The [Cyphesis](apps/cyphesis/README.md) server is used for hosting a virtual world.
* The [Ember](apps/ember/README.md) client is the main graphical client.
* [Eris](libs/eris/README.md) is a client support library.
* [Squall](libs/squall/README.md) provides features for syncing assets between servers and clients.
* [WFMath](libs/wfmath/README.md) contains various math related functions.
* [Mercator](libs/mercator/README.md) provides features for generating procedural terrain.
* [Atlas](libs/atlas/README.md) handles all communication between different components.
* [Varconf](libs/varconf/README.md) provides configuration support.

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

## Developing locally

We use Conan for our dependency handling. If you're developing locally you can issue this command to setup both a "
debug" and "release" environment.

```bash
conan install -s build_type=Debug . --build missing -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True --update  && conan install . --build missing -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True --update
```

## Running a server

To get a server running you need to build the project and then start the "cyphesis" artifact.

## Running a client

To get a client running you need to build the project and then start the "ember" artifact.

## Worlds data

This repository does not contain any of the demonstration worlds that we provide, since they often change a lot. They
can instead be found at [Worldforge Worlds](https://github.com/worldforge/worlds). If you build through Conan these
will be automatically installed and used. However, if you're setting up your own world you probably want to clone the
Worlds repository and use that instead. You can either configure Cyphesis to load from a different location by setting
WORLDFORGE_WORLDS_PATH at CMake time, or by altering the content of "cyphesis.vconf.in" ("autoimport" setting).

### Conan packages

We use Conan for our third party dependencies. In most cases we use the packages provided
by [Conan Center](https://conan.io/center), but in some cases we need to provide our own packages. These can all be
found in the [tools/conan-packages](tools/conan-packages) directory.

## License

All code is licensed under GPL v3+, unless otherwise stated.

## How to help

If you're interested in helping out with development you should check out these resources:

* [The main Worldforge site](http://worldforge.org/ "The main Worldforge site")
* [Bugs and feature planning on Launchpad](https://launchpad.net/worldforge "Worldforge Launchpad entry")
* [Gitter conversation](https://gitter.im/Worldforge/Lobby "Gitter conversation")
* [Worldforge wiki](http://wiki.worldforge.org "Worldforge wiki")
