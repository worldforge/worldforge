// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2003 Al Riddoch

// $Id$

#include <Atlas/Exception.h>

namespace Atlas {

const char * Exception::what() const throw()
{
    return m_description.c_str();
}

Exception::~Exception()
{
}

}
