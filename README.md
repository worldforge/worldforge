# Conan packages for Worldforge

These are the libraries used by Worldforge, provided as [Conan](https://www.conan.io) packages.

Most of them are adapted to be used specifically for Worldforge, so they might be missing some features that aren't needed by Worldforge specifically.

The libraries are automatically built by Travis and uploaded to the "worldforge/stable" channel at https://bintray.com/worldforge/worldforge-conan. To use them, configure conan as such
```
conan remote add worldforge https://api.bintray.com/conan/worldforge/worldforge-conan
```

Note that some packages require that Conan is run with Python 3.
