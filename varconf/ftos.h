/////////////////////////////////////////////////////////////////////////
//                               ftos.h
//
// Copyright (c) 1996 Bryce W. Harrington - bryce@neptune.net
//
//-----------------------------------------------------------------------
// License:  This code may be used by anyone for any purpose
//           so long as the copyright notices and this license
//           statement remains attached.
//-----------------------------------------------------------------------
// Description of file contents
// 1993 - Bryce Harrington
//    Created initial ftoa routine
//
// October 1996 - Bryce Harrington
//    Created itos from code taken from Kernighan & Ritchie
//       _The C Programming Language_ 2nd edition
//
// November 1996 - Bryce Harrington
//    Created new ftoa and ftos
//
// July 1999 - Bryce Harrington
//    Ported to Linux for use in WorldForge
//
/////////////////////////////////////////////////////////////////////////
#ifndef ftos_h
#define ftos_h

// ftos routine
const int FORCE_X10              = (1 << 0);
const int FORCE_DECIMAL          = (1 << 1);
const int FORCE_EXP_ZERO         = (1 << 2);
const int FORCE_HUNDRED_EXP_ZERO = (1 << 3);
const int FORCE_EXP_PLUS         = (1 << 4);
const int FORCE_EXPONENT         = (1 << 5);
const int FORCE_PLUS             = (1 << 6);

string ftos(double val, char mode='g', int sigfig=-1, int precision=-1, int options=0);

string itos(int n);

double round(double x);         // use rounding rule -> x to nearest int.
double round(double x, int k);  // round to the kth place

#endif // ftos_h

// #include "dtag.h" // for now
