# Writing code for Atlas-C++

Read this carefully before working on Atlas-C++.

## On writing templates

- put the declaration for the template class X in X.h
- put the definitions for the template class X in X_impl.h
- put any instantiations of X in X.cpp (or X.cc?), e.g.:
  template class X<int>;

This avoids duplicated instantiations and speeds up compile time. Note that this doesn't apply for absolutely all classes. If unsure, talk to somebody who knows more about templates.

## On filenames

- C++ implementation filenames will end in .cpp
- headers will end in .h

## On member names

Member functions will be lowerThenUpperCased(). Member variables will be prefixed with m_, and also be lowerThenUpperCased.

e.g.

```class Budgie { void cleanFeathers(); Feather* m_dirtyFeathers; };```

## On namespaces

Each subdirectory should use its own namespace for classes it contains, and package the object files in a library file associated with this namespace. namespaces are hierarchical following the directory hierarchy, and everything exists with the Atlas namespace.

Written by Stefanus Du Toit <mailto:sjdutoit@uwaterloo.ca> on 2001-01-14.

Added to by Alistair Riddoch <mailto:alriddoch@zepler.org> on 2001-05-11.

Last updated by Erik Ogenvik <mailto:erik@ogenvik.org> on 2017-11-25.

