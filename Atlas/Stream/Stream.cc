// This file is distributed under the GNU Lesser General Public license.
// Copyright (C) 2000 Michael Day (See the file COPYING for details).

#include "Stream.h"
	
void Atlas::Stream::Begin(const std::string& name, Type type)
{
    switch (type)
    {
	case LIST:
	break;

	case MAP:
	break;
    }
}

void Atlas::Stream::End()
{
}
	
void Atlas::Stream::Send(const std::string& name, int value)
{
}

void Atlas::Stream::Send(const std::string& name, float value)
{
}

void Atlas::Stream::Send(const std::string& name, const std::string& value)
{
}

void Atlas::Stream::Send(const std::string& name, const Object& value)
{
}
