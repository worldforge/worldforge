#ifndef VARCONF_PARSEERROR_H
#define VARCONF_PARSEERROR_H

#include <string>
#include <iostream>

namespace varconf {

/// May be thrown by parsing routines
class ParseError {
public:
  ParseError();
  ParseError(const ParseError& p);
  ParseError(const std::string& exp, int line, int col);
  
  virtual ~ParseError();

  friend std::ostream& operator<<(std::ostream& os, const ParseError& p);

private:
  std::string m_exp;
  int m_line, m_col;
};

} // namespace varconf

#endif
