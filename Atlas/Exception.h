// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2001 Al Riddoch

#ifndef ATLAS_EXCEPTION_H
#define ATLAS_EXCEPTION_H

#include <string>

namespace Atlas {

class Exception
{
  protected:
    std::string m_description;
    
  public:
    Exception(const std::string & d = "UNKNOWN ERROR") : m_description(d) { }
    const std::string & getDescription() const {
        return m_description;
    }
};

} // namespace Atlas

#endif // ATLAS_EXCEPTION_H
