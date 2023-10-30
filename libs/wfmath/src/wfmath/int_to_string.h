// int_to_string.h (Simple integer to std::string conversion)
//
//  The WorldForge Project
//  Copyright (C) 2001, 2002  The WorldForge Project
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
//  For information about WorldForge and its authors, please contact
//  the Worldforge Web Site at http://www.worldforge.org.

// Author: Ron Steinke
// Created: 2003-8-4

#ifndef WFMATH_INT_TO_STRING_H
#define WFMATH_INT_TO_STRING_H

#include <string>

namespace WFMath {

std::string IntToString(unsigned long);
std::string IntToString(long);
inline std::string IntToString(unsigned int val) {return IntToString((unsigned long) val);}
inline std::string IntToString(int val) {return IntToString((long) val);}
inline std::string IntToString(unsigned short val) {return IntToString((unsigned long) val);}
inline std::string IntToString(short val) {return IntToString((long) val);}

} // namespace WFMath

#endif // WFMATH_INT_TO_STRING_H
