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

#include <varconf/varconf_defs.h>
#include <iostream>
#include <string>

namespace varconf {

class VARCONF_API ParseError {
public:
  ParseError() = default;
  ParseError(const ParseError& p);
  ParseError(const std::string& exp, int line, int col);

  virtual ~ParseError();

  VARCONF_API friend std::ostream& operator<<(std::ostream& os, const ParseError& p);

  operator std::string();
  operator std::string() const;

private:
  std::string m_exp;
  int m_line{}, m_col{};
};

VARCONF_API std::ostream& operator<<(std::ostream& os, const ParseError& p);

} // namespace varconf

#endif
