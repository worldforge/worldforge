// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit, Michael Day

// $Id$

/** @file Codecs/Utility.cpp
 * Various utility functions for codec implementation.
 *
 * This file implements various functions that can be used in implementing an
 * Atlas::Codec.
 *
 * @see Atlas::Codec
 * @author Stefanus Du Toit <sdt@gmx.net>, Michael Day <mikeday@corplink.com.au>
 */

#include <Atlas/Codecs/Utility.h>

#include <algorithm>

namespace Atlas { namespace Codecs {

/** Escape a string by converting certain characters to their hexadecimal
 * value.
 *
 * @return The escaped message.
 * @param prefix The string that is to be prepended to the hexadecimal value.
 * @param special The characters that are to be escaped.
 * @param message The message that is to be escaped.
 * @see hexDecode
 */
const std::string hexEncodeWithPrefix(const std::string& prefix,
                                      const std::string& special,
                                      const std::string& message)
{
    std::string encoded;

    for (std::string::const_iterator i = message.begin(); i != message.end(); ++i)
    {
        if (std::find(special.begin(), special.end(), *i) != special.end())
        {
            encoded += prefix;
            encoded += charToHex(*i);
        }
        else
        {
            encoded += *i;
        }
    }

    return encoded;
}

/** Parse a message and replace hexadecimal 'escaped' values with their ASCII
 * counterparts.
 *
 * This function does the opposite to hexDecode - it takes a message that was
 * prepared with hexDecode, searches for occurences of prefix and replaces the
 * following hexadecimal values with their ASCII counterparts.
 *
 * @return The unescaped string.
 * @param prefix The string that is followed by the escaped characters
 * @param message The escaped message.
 */
const std::string hexDecodeWithPrefix(const std::string& prefix,
                                      const std::string& message)
{
    std::string newMessage;
    std::string curFragment;
    
    for (size_t i = 0; i < message.size(); i++) {
        if (std::equal(prefix.begin(),prefix.begin() + curFragment.length() + 1,
                    (curFragment + message[i]).begin())) {
            curFragment += message[i];
        } else {
            newMessage += curFragment + message[i];
            curFragment = "";
        }
        if (curFragment == prefix) {
            std::string hex;
            hex += message[++i];
            hex += message[++i];
            newMessage += hexToChar(hex);
            curFragment = "";
        }
    }

    return newMessage;
}

} } // namespace Atlas::Codecs
