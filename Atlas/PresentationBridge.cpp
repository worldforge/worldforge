/*
 Copyright (C) 2013 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "PresentationBridge.h"

namespace Atlas
{

PresentationBridge::PresentationBridge(std::stringstream& stream) :
		mStream(stream)
{
    mStream.precision(6);
}

PresentationBridge::~PresentationBridge()
{
}

void PresentationBridge::streamBegin()
{
	addPadding();
}
void PresentationBridge::streamMessage()
{
	addPadding();
}
void PresentationBridge::streamEnd()
{
	removePadding();
}

void PresentationBridge::mapMapItem(const std::string& name)
{
	mStream << mPadding << name << std::endl;
	addPadding();
}
void PresentationBridge::mapListItem(const std::string& name)
{
	mMapsInList.push(0);
	mStream << mPadding << name << std::endl;
	addPadding();
}
void PresentationBridge::mapIntItem(const std::string& name, long i)
{
	mStream << mPadding << name << ": " << i << std::endl;
}
void PresentationBridge::mapFloatItem(const std::string& name, double d)
{
	mStream << mPadding << name << ": " << std::fixed << d << std::endl;
}
void PresentationBridge::mapStringItem(const std::string& name, const std::string& s)
{
	mStream << mPadding << name << ": " << s << std::endl;
}
void PresentationBridge::mapEnd()
{
	removePadding();
}

void PresentationBridge::listMapItem()
{
	//Check if we've already printed a map for this list, and if so print a separator
	int items = mMapsInList.top();
	if (items) {
		mStream << mPadding << "---" << std::endl;
	}
	mMapsInList.pop();
	mMapsInList.push(items + 1);
	addPadding();
}
void PresentationBridge::listListItem()
{
	mMapsInList.push(0);
	addPadding();
}
void PresentationBridge::listIntItem(long i)
{
	mStream << mPadding << ": " << i << std::endl;
}
void PresentationBridge::listFloatItem(double d)
{
	mStream << mPadding << ": " << d << std::endl;
}
void PresentationBridge::listStringItem(const std::string& s)
{
	mStream << mPadding << ": " << s << std::endl;
}
void PresentationBridge::listEnd()
{
	mMapsInList.pop();
	removePadding();
}
void PresentationBridge::addPadding()
{
	mPadding += "  ";
}

void PresentationBridge::removePadding()
{
	mPadding.erase(mPadding.end() - 2, mPadding.end());
}

}
