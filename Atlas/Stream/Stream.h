// This file is distributed under the GNU Lesser General Public license.
// Copyright (C) 2000 Michael Day (See the file COPYING for details).

#ifndef ATLAS_STREAM_STREAM_H
#define ATLAS_STREAM_STREAM_H

#include "../Object/Object.h"

#include <string>

namespace Atlas
{
    namespace Stream
    {
	enum Type
	{
	    LIST,
	    MAP,
	};
	
	void Begin(const std::string& name, Type);
	void End();
	
	void Send(const std::string& name, int value);
	void Send(const std::string& name, float value);
	void Send(const std::string& name, const std::string& value);
	void Send(const std::string& name, const Object& value);
    }
}

#endif
