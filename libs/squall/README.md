# Squall

[![Join us on Gitter!](https://badges.gitter.im/Worldforge.svg)](https://gitter.im/Worldforge/Lobby)
[![Build Squall standalone](https://github.com/worldforge/worldforge/actions/workflows/build-squall.yml/badge.svg)](https://github.com/worldforge/worldforge/actions/workflows/build-squall.yml)

Squall is a library used for making sure that clients have up-to-date content. It handles both syncing as well as
partial updates.

It's meant to be used in an environment where there's a need to have multiple clients which can download content from
multiple servers, and where there's a need to easily signal to the clients when content has changed.

Squall is part of the [WorldForge](http://worldforge.org/ "The main Worldforge site") virtual world system, but can be
used independently.

## Installation

Squall is build along with the main Worldforge system. There are instructions to be found [here](../../README.md).

However, you might not want to build all of Worldforge just to build Squall. In that case you can build Squall from this
directory with these commands:

```bash
conan install tools/conan --build missing
cmake --preset conan-release -DCMAKE_INSTALL_PREFIX=./build/install/release
cmake --build --preset conan-release -j --target all
cmake --build --preset conan-release -j --target install
```

You can then build and install. This will also enable "library installation" where libsquall.a|so|dll will be
installed (which it won't be if built from the root).

## Description

Squall is meant to allow for a client to sync data from one or many servers, where there are multiple versions of the
same data, but with various smaller alterations. A typical example would be a Worldforge client which connects to
multiple game servers, where a lot of media is the same, but each server has some extra media.

The main idea is that all content is identified by hashing of the data. All data is stored in a central location, in a
repository. When using the data a client can either traverse the repository directly (like a virtual file system), or "
realize" the content onto the file system as either copied data or soft or hard links.

### Manifests

The basic method of describing data is through "manifests". These are simple text files that describe the content of a
directory. Each entry in the directory is either a file or a subdirectory. In the Manifest this is expressed with a hash
of the content, the size of the content and the name of the file/directory.

Each Manifest entry always start with the version of the format. Future versions of this library might introduce new
features which would make it incompatible with older versions.

Here's an example of a Manifest:

```
1
c412317b3a479cdcee9e9cfbe0666db34683e5bc27e61264e446abaed0dfa9c3 1 bar/
4355a46b19d348dc2f57c046f8ef63d4538ebb93600f3c9ee954a2746dd865 0 empty_directory/
e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855 0 empty_file
```

If a file name ends with "/" it denotes a directory.

## License

Squall is licensed under either the MIT License or the GPL v3+, at your choice. This makes this library stand apart from
other Worldforge libraries. The intention is to allow for Squall to be used outside of Worldforge, since we think it's
neat.

Note that the BLAKE3 hashing algorithm that we use is licensed dually under either CC0 1.0 or Apache License 2.0.

## Design

Since we had a couple of extra requirements we didn't want to just use basic file transfers for syncing media. The main
requirements we had were:

* Allow the client to connect to different servers, each with slightly different media. The idea here is that we provide
  base media, and then will let servers extend this with their own media. We therefore envision that a lot of the media
  will be similar. So for example, a client would connect to two different servers, which share 90% of common assets.
  These common assets wouldn't need to be re-downloaded.
* Have a built-in mechanism for when assets are updated. This ties in to the vision we have about a development loop
  where the world is created inside a running server, without the need to restart the server. To accomplish this we need
  a good way to inform the client about changed assets. Whenever something changes the client should download the
  changed asset only.
* Use simple and tested existing file transfer mechanisms. Such as HTTP. We want to make it simple to expose assets
  using a basic file server.

### Implementation

All of these requirements led to the design of Squall. It borrows ideas from both Git and BitTorrent, in the sense that
all assets are represented by hashes of their content. This also goes for directories.

An effect of this is that a client only needs to download any hashes that it's missing. Thus fulfilling requirement #1.

Since directories also are computed using hashes of their content, any change to an assets will resonate to all its
parent directories and in the end to the root directory. Thus, every change will result in a new hash for the root,
which can be communicated to the client. In that way fulfilling requirement #2.

And since we only need to expose data attached to hashes it's easy to serve data using a standard file server. Thus
fulfilling requirement #3.

## CLI

We also provide a CLI tool. It's installed by default. To see available commands run ```squall --help```.
All squall commands require the "--repository" flag which points to a directory in which a Squall repository is stored.

For example, to generate a digest from an existing file structure:

```bash
squall --repository /path/to/repo generate --source /path/to/assets 
```

To add a new root from a generated digest:

```bash
squall --repository /path/to/repo root add --root a_name_for_a_root --signature a_signature_created_by_a_call_to_generate
```

To download assets from a remote repository into a local repository:

```bash
squall --repository /path/to/repo root add --root a_name_for_a_root --signature a_signature
```

To realize a file hierarchy in the local file system.:

```bash
squall --repository /path/to/repo root --destination=/path/where/files/should/be/created --signature a_signature
```

## How to help

If you're interested in helping out with development you should check out these resources:

* [The main Worldforge site](http://worldforge.org/ "The main Worldforge site")
* [Gitter conversation](https://gitter.im/Worldforge/Lobby "Gitter conversation")

