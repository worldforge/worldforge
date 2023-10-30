// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2001 Al Riddoch

// $Id$

#ifndef ATLAS_EXCEPTION_H
#define ATLAS_EXCEPTION_H

#include <string>
#include <stdexcept>

namespace Atlas {

/// Base class for all exceptions thrown by Atlas-C++. All subclasses
/// should pass in a description of the exceptional circumstance encountered.
class Exception : public std::runtime_error
{

  public:
    explicit Exception(const std::string & d = "UNKNOWN ERROR") noexcept
			: std::runtime_error(d) { }

    ~Exception() noexcept override = default;

    std::string getDescription() const {
        return what();
    }
};

} // namespace Atlas

#endif // ATLAS_EXCEPTION_H
