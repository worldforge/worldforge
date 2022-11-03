# Squall

[![Join us on Gitter!](https://badges.gitter.im/Worldforge.svg)](https://gitter.im/Worldforge/Lobby)

Squall is a library used for making sure that clients have up to date content. It handles both syncing as well as partial updates.

It's meant to be used in an environment where there's a need to have multiple clients which can download content from multiple servers, and where there's a need to easily signal to the clients when content has changed.

Squall is part of the [WorldForge](http://worldforge.org/ "The main Worldforge site") virtual world system, but can be used independently.

## Installation

The simplest way to install all required dependencies is by using [Conan](https://www.conan.io).

```bash
mkdir build && cd build
conan install .. --build missing
cmake ..
make -j all install
```

### Tests

The test suite can be built and run using the ```check``` target. For example:

```bash
make check
```

## How to help

If you're interested in helping out with development you should check out these resources:

* [The main Worldforge site](http://worldforge.org/ "The main Worldforge site")
* [Gitter conversation](https://gitter.im/Worldforge/Lobby "Gitter conversation")

