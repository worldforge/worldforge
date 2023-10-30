/*
 *  parse_error.h - interface for parse error handling class.
 *  Copyright (C) 2001, Stefanus Du Toit, Joseph Zupko
 *            (C) 2004 Alistair Riddoch
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
 
#ifndef VARCONF_PARSEERROR_H
#define VARCONF_PARSEERROR_H

#include "varconf_defs.h"
#include <iostream>
#include <string>
#include <stdexcept>

namespace varconf {

class VARCONF_API ParseError : public std::runtime_error {
public:
  ParseError(const ParseError& p) = default;
  ParseError(const std::string& exp, int line, int col);

  ~ParseError() override;

};


} // namespace varconf

#endif
