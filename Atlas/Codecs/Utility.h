// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit

#include <cstdio>

using namespace std;

inline const string charToHex(char c)
{
    char hex[3];
    snprintf(hex, 3, "%x", c);
    return hex;
}

inline const string hexEncode(const string& prefix, const string& special,
        const string& postfix, const string& message)
{
    string newMessage;
    
    for (int i = 0; i < message.size(); i++) {
        if (find(special.begin(), special.end(), message[i]) != special.end()) {
            newMessage += prefix;
            newMessage += charToHex(message[i]);
            newMessage += postfix;
        }
        newMessage += message[i];
    }

    return newMessage;
}
