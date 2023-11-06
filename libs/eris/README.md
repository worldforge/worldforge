# Eris, the WorldForge client entity library

[![Join us on Gitter!](https://badges.gitter.im/Worldforge.svg)](https://gitter.im/Worldforge/Lobby)
[![Appveyor build status](https://ci.appveyor.com/api/projects/status/github/worldforge/eris?branch=master&svg=true)](https://ci.appveyor.com/project/erikogenvik/eris)
[![Travis build Status](https://travis-ci.com/worldforge/eris.svg?branch=master)](https://travis-ci.com/worldforge/eris)
[![Github build status](https://github.com/worldforge/eris/actions/workflows/cmake.yml/badge.svg)](https://github.com/worldforge/eris/actions/workflows/cmake.yml)

Eris is a client library for the [WorldForge](http://worldforge.org/ "The main Worldforge site") system. It's meant to
be used by any client which wishes to easily interact with a Worldforge server.

It handles the client-side implementation of the meta-server protocol, and
querying game servers; out-of-game (OOG) operations (via the Lobby and Rooms), 
and most importantly, in-game (IG) operations such as entity creation, movement 
and attribute updates.

## Usage

Eris provides a generic 'Entity' class, which you are free to sub-class and
provide to the system (by registering a factory); thus you are free to create
different classes to handle characters, walls, vehicles, etc as your client
dictates. An alternative approach is to create peer classes, and connect them
to Eris via callbacks. Eris makes extensive use of libSigC++, which must be
correctly installed and functioning on your system.

## How to help

If you're interested in helping out with development you should check out these resources:

* [The main Worldforge site](http://worldforge.org/ "The main Worldforge site")
* [Bugs and feature planning on Launchpad](https://launchpad.net/eris "Eris Launchpad entry")
* [Gitter conversation](https://gitter.im/Worldforge/Lobby "Gitter conversation")
* [Mailing lists](http://mail.worldforge.org/lists/listinfo/ "Mailing lists")
