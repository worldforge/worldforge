//
//  CxxDebug.hxx
//
//  Copyright (c) 2008 Barry A. Scott
//
#ifndef CXX_Debug_hxx
#define CXX_Debug_hxx

//
//  Functions useful when debugging PyCXX
//
#ifdef PYCXX_DEBUG
extern void bpt();
extern void printRefCount( PyObject *obj );
#endif

#endif
