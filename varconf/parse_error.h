/*
 *  parse_error.h - interface for parse error handling class.
 *  Copyright (C) 2000, Stefanus Du Toit, Joseph Zupko
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  For more information on the GPL, please go to: 
 *  http://www.gnu.org/copyleft/gpl.html
 *
 *      Contact:  Joseph Zupko
 *                jaz147@psu.edu
 *
 *                189 Reese St.
 *                Old Forge, PA 18518
 */

#ifndef VARCONF_PARSEERROR_H
#define VARCONF_PARSEERROR_H

#include <iostream>
#include <string>

namespace varconf {

class ParseError {
public:
  ParseError() {}
  ParseError( const ParseError& p);
  ParseError( const std::string& exp, int line, int col);

  virtual ~ParseError() {}

  friend std::ostream& operator<<( std::ostream& os, const ParseError& p);

  operator std::string();

private:
  std::string m_exp;
  int m_line, m_col;
};

} // namespace varconf

#endif
