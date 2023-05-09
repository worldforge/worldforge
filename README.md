# Conan packages for Worldforge

[![Join us on Gitter!](https://badges.gitter.im/Worldforge.svg)](https://gitter.im/Worldforge/Lobby)
[![Appveyor build status](https://ci.appveyor.com/api/projects/status/github/worldforge/conan-packages?branch=master&svg=true)](https://ci.appveyor.com/project/erikogenvik/conan-packages)
[![Travis build Status](https://travis-ci.com/worldforge/conan-packages.svg?branch=master)](https://travis-ci.com/worldforge/conan-packages)
[![CMake](https://github.com/worldforge/conan-packages/actions/workflows/cmake.yml/badge.svg)](https://github.com/worldforge/conan-packages/actions/workflows/cmake.yml)

These are the libraries used by Worldforge, provided as [Conan](https://www.conan.io) packages.

Most of them are adapted to be used specifically for Worldforge, so they might be missing some features that aren't needed by Worldforge specifically.

The libraries are automatically built by Travis and uploaded to the "worldforge/stable" channel at https://artifactory.ogenvik.org/ui/repos/tree/General/conan%2Fworldforge. To use them, configure conan as such
```
conan remote add worldforge https://artifactory.ogenvik.org/artifactory/api/conan/conan
```

Note that some packages require that Conan is run with Python 3.

## Install locally

To install any of these packages locally, you would in most cases do:
```bash
conan create ./<package> --build missing
```
