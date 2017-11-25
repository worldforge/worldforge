Read this carefully before working on Atlas-C++.

# On writing templates:

 - put the declaration for the template class X in X.h
 - put the definitions for the template class X in X_impl.h
 - put any instantiations of X in X.cpp (or X.cc?), e.g.:
     template class X<int>;

This avoids duplicated instantiations and speeds up compile time.
Note that this doesn't apply for absolutely all classes. If unsure,
talk to somebody who knows more about templates.

# On filenames:

 - C++ implementation filenames will end in .cpp
 - headers will end in .h
 - files may be lower cased. This is up to debate.

# On member names:

Member functions will be lowerThenUpperCased(). Member
variables will be prefixed with m_, and also be lowerThenUpperCased.

e.g.

```class Budgie { void cleanFeathers(); Feather* m_dirtyFeathers; };```

# On namespaces:

Each subdirectory should use its own namespace for classes it contains,
and package the object files in a library file associated with
this namespace. namespaces are hierarchical following the directory
hierarchy, and everything exists with the Atlas namespace.

From 0.5.x onwards, the "using namespace ..." directive should never be used
is it increases the likelyhood on namespace pollution, and slows down compile
times. Use is also discouraged in programs which use Atlas-C++, and 
in any C++ program. No additional use should be made of the
"using namespace ..." directive should be made in branch before 0.5.x, and
its use may be phased out in these branches eventually.

Written by Stefanus Du Toit <sjdutoit@uwaterloo.ca> on 2001-01-14.

Added to by Alistair Riddoch <alriddoch@zepler.org> on 2001-05-11.

Last updated by Erik Ogenvik <erik@ogenvik.org> on 2017-11-25.

