// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2001 Al Riddoch

#ifndef ATLAS_EXCEPTION_H
#define ATLAS_EXCEPTION_H

#include <string>

namespace Atlas {

/*%TODO(Jesse,Atlas,Exception)
* Exception should descend from std::exception.
*/
class Exception
{
  protected:
    std::string description;
  public:
    Exception(const std::string & d = "UNKNOWN ERROR") : description(d) { }
    const std::string & getDescription() const {
        return description;
    }
};

} // namespace Atlas

#endif // ATLAS_EXCEPTION_H
