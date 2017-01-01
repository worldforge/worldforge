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
		mStream(stream), mMaxItemsPerLevel(0), mIsSkipEntry(false), mStartFilterLevel(1)
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
	if (checkAndUpdateMaxItemCounter()) {
        mStream << mPadding << name << std::endl;
	}
	addPadding();
}
void PresentationBridge::mapListItem(const std::string& name)
{
	if (checkAndUpdateMaxItemCounter()) {
        mStream << mPadding << name << std::endl;
	}
	mMapsInList.push(0);
	addPadding();
}
void PresentationBridge::mapIntItem(const std::string& name, long i)
{
	if (checkAndUpdateMaxItemCounter()) {
        mStream << mPadding << name << ": " << i << std::endl;
	}
}
void PresentationBridge::mapFloatItem(const std::string& name, double d)
{
	if (checkAndUpdateMaxItemCounter()) {
        mStream << mPadding << name << ": " << std::fixed << d << std::endl;
	}
}
void PresentationBridge::mapStringItem(const std::string& name, const std::string& s)
{
	if (checkAndUpdateMaxItemCounter()) {
        mStream << mPadding << name << ": " << s << std::endl;
	}
}
void PresentationBridge::mapEnd()
{
	removePadding();
}

void PresentationBridge::listMapItem()
{
	int items = mMapsInList.top();
	if (checkAndUpdateMaxItemCounter()) {
        //Check if we've already printed a map for this list, and if so print a separator
		if (items) {
			mStream << mPadding << "---" << std::endl;
		}
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
	if (checkAndUpdateMaxItemCounter()) {
        mStream << mPadding << ": " << i << std::endl;
	}
}
void PresentationBridge::listFloatItem(double d)
{
	if (checkAndUpdateMaxItemCounter()) {
        mStream << mPadding << ": " << d << std::endl;
	}
}
void PresentationBridge::listStringItem(const std::string& s)
{
	if (checkAndUpdateMaxItemCounter()) {
        mStream << mPadding << ": " << s << std::endl;
	}
}
void PresentationBridge::listEnd()
{
	mMapsInList.pop();
	removePadding();
}
void PresentationBridge::addPadding()
{
	mEntriesPerLevelCounter.push_back(0);
	mPadding += "  ";
}

void PresentationBridge::removePadding()
{
    size_t itemsThisLevel = mEntriesPerLevelCounter.back();
    bool wasSkipping = mIsSkipEntry;
	mEntriesPerLevelCounter.pop_back();
	if (mMaxItemsPerLevel) {
        size_t level = 0;
		for (size_t entry : mEntriesPerLevelCounter) {
			mIsSkipEntry = entry >= mMaxItemsPerLevel && level >= mStartFilterLevel;
            level++;
		}
	}

    if (wasSkipping != mIsSkipEntry) {
        mStream << mPadding << "... (" << (itemsThisLevel - mMaxItemsPerLevel) << " more items) ..." << std::endl;
    }

	mPadding.erase(mPadding.end() - 2, mPadding.end());
}

void PresentationBridge::setMaxItemsPerLevel(size_t maxItems)
{
	mMaxItemsPerLevel = maxItems;
}

void PresentationBridge::setStartFilteringLevel(size_t startFilteringLevel)
{
    mStartFilterLevel = startFilteringLevel;
}

bool PresentationBridge::checkAndUpdateMaxItemCounter()
{

	if (mMaxItemsPerLevel && !mEntriesPerLevelCounter.empty() && mEntriesPerLevelCounter.size() > mStartFilterLevel) {
		size_t& itemsInLevel = mEntriesPerLevelCounter.back();
		itemsInLevel++;
		if (itemsInLevel >= mMaxItemsPerLevel) {
			mIsSkipEntry = true;
		}
        if (mIsSkipEntry) {
            return false;
        }
	}
	return true;
}


}
