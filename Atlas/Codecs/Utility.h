// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit, Michael Day

// $Id$

#ifndef ATLAS_CODECS_UTILITY_H
#define ATLAS_CODECS_UTILITY_H

/** @file Codecs/Utility.h
 * Various utility functions for codec implementation.
 *
 * This file contains various functions that can be used in implementing an
 * Atlas::Codec.
 *
 * @see Atlas::Codec
 * @author Stefanus Du Toit <sdt@gmx.net>, Michael Day <mikeday@corplink.com.au>
 */

#include <cstdio>
#include <string>

namespace Atlas { namespace Codecs {

/// Convert an ASCII char to its hexadecimal value
inline const std::string charToHex(char c)
{
    char hex[3];
#ifdef _WIN32
    _snprintf(hex, 3, "%x", c);
#else
    snprintf(hex, 3, "%x", c);
#endif
    return hex;
}

/// Convert a string with a hexadecimal value (2 characters) to an ASCII char
inline char hexToChar(const std::string& hex)
{
    int c;
    if (sscanf(hex.c_str(), "%x", &c) == 1) {
        return (char) c;
    } else {
        return (char) 0;
    }
}

const std::string hexEncodeWithPrefix(const std::string& prefix,
                                      const std::string& special,
                                      const std::string& message);

const std::string hexDecodeWithPrefix(const std::string& prefix,
                                      const std::string& message);

} } // namespace Atlas::Codecs

#endif // ATLAS_CODECS_UTILITY_H
