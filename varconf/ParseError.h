#ifndef PARSEERROR_H
#define PARSEERROR_H

#include <string>
#include <iostream>

/// May be thrown by parsing routines
class ParseError {
public:
  ParseError();
  ParseError(const ParseError& p);
  ParseError(const string& exp, int line, int col);
  
  virtual ~ParseError();

  friend ostream& operator<<(ostream& os, const ParseError& p);

private:
  string m_exp;
  int m_line, m_col;
};

#endif
