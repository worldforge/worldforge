// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Stefanus Du Toit, Michael Day

#include <cstdio>
#include <string>
#include <algorithm>

inline const string charToHex(char c)
{
    char hex[3];
    snprintf(hex, 3, "%x", c);
    return hex;
}

inline char hexToChar(const string& hex)
{
    int c;
    sscanf(hex.c_str(), "%x", &c);
    return c;
}

inline const string hexEncode(const string& prefix, const string& special,
        const string& message)
{
    string encoded;

    for (string::const_iterator i = message.begin(); i != message.end(); ++i)
    {
	if (find(special.begin(), special.end(), *i) != special.end())
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

inline const string hexDecode(const string& prefix, const string& message)
{
    string newMessage;
    string curFragment;
    
    for (int i = 0; i < message.size(); i++) {
        if (equal(prefix.begin(), prefix.begin() + curFragment.length() + 1, 
                    (curFragment + message[i]).begin())) {
            curFragment += message[i];
        } else {
            newMessage += curFragment + message[i];
            curFragment = "";
        }
        if (curFragment == prefix) {
            string hex;
            hex += message[++i];
            hex += message[++i];
            newMessage += hexToChar(hex);
            curFragment = "";
        }
    }

    return newMessage;
}
