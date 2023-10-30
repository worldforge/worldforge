/*
 *  parse_error.cpp - implementation of the parse error handling class
 *  Copyright (C) 2001, Stefanus Du Toit, Joseph Zupko
 *            (C) 2003-2004 Alistair Riddoch
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 *  Contact:  Joseph Zupko
 *            jaz147@psu.edu
 *
 *            189 Reese St.
 *            Old Forge, PA 18518
 */
 
#include "parse_error.h"

#include <iostream>
#include <sstream>

namespace {
std::string compose(const std::string& exp, int line, int col) {
	std::stringstream ss;
	ss << "ParseError: Expected " << exp << " at line " << line
		  << ", column " << col << ".";
	return ss.str();
}
}

namespace varconf {

ParseError::ParseError( const std::string& exp, int line, int col)
: std::runtime_error(compose(exp, line, col))
{
}

ParseError::~ParseError() = default;


} // namespace varconf
