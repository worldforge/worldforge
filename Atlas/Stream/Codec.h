// This file may be redistributed and modified under the terms of the
// GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Michael Day

#ifndef ATLAS_STREAM_CODEC_H
#define ATLAS_STREAM_CODEC_H

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
