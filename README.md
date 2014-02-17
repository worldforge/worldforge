#Eris, the WorldForge client entity library

Eris is a client library for the Worldforge system. It's meant to be used by
any client which wishes to easily interact with a Worldforge server.

It handles the client-side implementation of the meta-server protocol, and
querying game servers; out-of-game (OOG) operations (via the Lobby and Rooms), 
and most importantly, in-game (IG) operations such as entity creation, movement 
and attribute updates.

##Installation

We strongly suggest that you use the [Hammer](http://wiki.worldforge.org/wiki/Hammer_Script "The Hammer script") tool to compile Eris.
This is script provided by the Worldforge project which will download and install all of the required libraries and components used by Worldforge.

The INSTALL document also contains information on how to compile Eris.

##Usage

Eris provides a generic 'Entity' class, which you are free to sub-class and
provide to the system (by registering a factory); thus you are free to create
different classes to handle characters, walls, vehicles, etc as your client
dictates. An alternative approach is to create peer classes, and connect them
to Eris via callbacks. Eris makes extensive use of libSigC++, which must be
correctly installed and functioning on your system.

##How to help

If you're interested in helping out with development you should check out these resources:

* [The main Worldforge site](http://worldforge.org/ "The main Worldforge site")
* [Bugs and feature planning on Launchpad](https://launchpad.net/eris "Eris Launchpad entry")
* [IRC channels](http://worldforge.org/doc/irc "IRC channels")
* [Mailing lists](http://mail.worldforge.org/lists/listinfo/ "Mailing lists")
