[![Download](https://api.bintray.com/packages/bincrafters/public-conan/bullet3%3Abincrafters/images/download.svg) ](https://bintray.com/bincrafters/public-conan/bullet3%3Abincrafters/_latestVersion)
[![Build Status](https://travis-ci.com/bincrafters/conan-bullet3.svg?branch=stable%2F2.88)](https://travis-ci.com/bincrafters/conan-bullet3)
[![Build status](https://ci.appveyor.com/api/projects/status/github/bincrafters/conan-bullet3?branch=stable%2F2.88&svg=true)](https://ci.appveyor.com/project/bincrafters/conan-bullet3)

[Conan.io](https://conan.io) package recipe for [*bullet3*](https://github.com/bulletphysics/bullet3).

Bullet Physics SDK: real-time collision detection and multi-physics simulation for VR, games, visual effects, robotics, machine learning etc.

The packages generated with this **conanfile** can be found on [Bintray](https://bintray.com/bincrafters/public-conan/bullet3%3Abincrafters).

## For Users: Use this package

### Basic setup

    $ conan install bullet3/2.88@bincrafters/stable

### Project setup

If you handle multiple dependencies in your project is better to add a *conanfile.txt*

    [requires]
    bullet3/2.88@bincrafters/stable

    [generators]
    cmake

Complete the installation of requirements for your project running:

    $ mkdir build && cd build && conan install ..

Note: It is recommended that you run conan install from a build directory and not the root of the project directory.  This is because conan generates *conanbuildinfo* files specific to a single build configuration which by default comes from an autodetected default profile located in ~/.conan/profiles/default .  If you pass different build configuration options to conan install, it will generate different *conanbuildinfo* files.  Thus, they should not be added to the root of the project, nor committed to git.

## For Packagers: Publish this Package

The example below shows the commands used to publish to bincrafters conan repository. To publish to your own conan respository (for example, after forking this git repository), you will need to change the commands below accordingly.

## Build and package

The following command both runs all the steps of the conan file, and publishes the package to the local system cache.  This includes downloading dependencies from "build_requires" and "requires" , and then running the build() method.

    $ conan create bincrafters/stable


### Available Options
| Option        | Default | Possible Values  |
| ------------- |:----------------- |:------------:|
| shared      | False |  [True, False] |
| fPIC      | True |  [True, False] |
| bullet3      | False |  [True, False] |
| graphical_benchmark      | False |  [True, False] |
| double_precision      | False |  [True, False] |
| bt2_thread_locks      | False |  [True, False] |
| btSoftMultiBodyDynamicsWorld      | False |  [True, False] |
| pybullet      | False |  [True, False] |
| pybullet_numpy      | False |  [True, False] |
| network_support      | False |  [True, False] |

## Add Remote

    $ conan remote add bincrafters "https://api.bintray.com/conan/bincrafters/public-conan"

## Upload

    $ conan upload bullet3/2.88@bincrafters/stable --all -r bincrafters


## Conan Recipe License

NOTE: The conan recipe license applies only to the files of this recipe, which can be used to build and package bullet3.
It does *not* in any way apply or is related to the actual software being packaged.

[MIT](https://github.com/bincrafters/conan-bullet3.git/blob/stable/2.88/LICENSE)
