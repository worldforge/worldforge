# Squall

[![Join us on Gitter!](https://badges.gitter.im/Worldforge.svg)](https://gitter.im/Worldforge/Lobby)

Squall is a library used for making sure that clients have up to date content. It handles both syncing as well as partial updates.

It's meant to be used in an environment where there's a need to have multiple clients which can download content from multiple servers, and where there's a need to easily signal to the clients when content has changed.

Squall is part of the [WorldForge](http://worldforge.org/ "The main Worldforge site") virtual world system, but can be used independently.

## Installation

The simplest way to install all required dependencies is by using [Conan](https://www.conan.io).

```bash
mkdir build && cd build
conan install ../tools/conan --build missing
cmake ..
make -j all install
```

### Tests

The test suite can be built and run using the ```check``` target. For example:

```bash
make check
```

## Description

Squall is meant to allow for a client to sync data from one or many servers, where there are multiple versions of the same data, but with various smaller alterations. A typical example would be a Worldforge client which connects to multiple game servers, where a lot of media is the same, but each server has some extra media.

The main idea is that all content is identified by hashing of the data. All data is stored in a central location, in a repository. When using the data a client can either traverse the repository directly (like a virtual file system), or "realize" the content onto the file system as either copied data or soft or hard links.

### Records

The basic method of describing data is through "records". These are simple text files that describe the content of a directory. Each entry in the directory is either a file or a subdirectory. In the Record this is expressed with a hash of the content, the size of the content and the name of the file/directory.

Each Record entry always start with the version of the format. Future versions of this library might introduce new features which would make it incompatible with older versions.

Here's an example of a Record:

```
1
c412317b3a479cdcee9e9cfbe0666db34683e5bc27e61264e446abaed0dfa9c3 1 bar/
4355a46b19d348dc2f57c046f8ef63d4538ebb93600f3c9ee954a2746dd865 0 empty_directory/
e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855 0 empty_file
```

If a file name ends with "/" it denotes a directory.

## How to help

If you're interested in helping out with development you should check out these resources:

* [The main Worldforge site](http://worldforge.org/ "The main Worldforge site")
* [Gitter conversation](https://gitter.im/Worldforge/Lobby "Gitter conversation")

