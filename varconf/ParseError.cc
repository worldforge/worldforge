#include <iostream>
#include "ParseError.h"

using namespace std;

namespace varconf {

ParseError::ParseError()
{
}

ParseError::ParseError(const ParseError& p)
 : m_exp(p.m_exp), m_line(p.m_line), m_col(p.m_col)
{
}

ParseError::ParseError(const string& exp, int line, int col)
 : m_exp(exp), m_line(line), m_col(col)
{
}

ParseError::~ParseError()
{
}

ostream& operator<<(ostream& os, const ParseError& p)
{
  return (os << "ParseError: Expected " << p.m_exp << " at line " << p.m_line
          << ", column " << p.m_col << "." << endl);
}

}
