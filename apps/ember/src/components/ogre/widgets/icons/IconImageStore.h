//
// C++ Interface: IconImageStore
//
// Description: 
//
//
// Author: Erik Ogenvik <erik@ogenvik.org>, (C) 2007
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.//
//
#ifndef EMBEROGRE_GUI_ICONSICONIMAGESTORE_H
#define EMBEROGRE_GUI_ICONSICONIMAGESTORE_H

#include <string>
#include <stack>
#include <vector>

namespace CEGUI {
class Image;

class Texture;
}

namespace Ogre {
class Image;
}


namespace Ember::OgreView::Gui::Icons {

class IconImageStore;

class IconImageStoreEntry {
public:
	typedef std::pair<size_t, size_t> PixelPos;

	IconImageStoreEntry(IconImageStore& iconImageStore, PixelPos pixelPosInImageset);

	virtual ~IconImageStoreEntry();

	const CEGUI::Image* getImage();

	const CEGUI::Image* getImage() const;

	const std::string& getImageName();

	const std::string& getImageName() const;

	Ogre::Box getBox() const;

	Ogre::TRect<float> getRelativeBox() const;

	Ogre::TexturePtr getTexture();

	//Ogre::PixelBox getImagePixelBox();

//     Ogre::Image::PixelBox getImageBox();

protected:
	void createImage();

	CEGUI::Image* mImage;
	IconImageStore& mIconImageStore;
	PixelPos mPixelPosInImageset;
	std::string mImageName;
};

/**
	@author Erik Ogenvik <erik@ogenvik.org>
*/
class IconImageStore {
public:
	friend class IconImageStoreEntry;

	//typedef std::map<int, IconImageStoreEntry*> IconImageStoreEntryMap;
	typedef std::stack<IconImageStoreEntry*> IconImageStoreEntryStack;
	typedef std::vector<IconImageStoreEntry*> IconImageStoreEntryStore;

	/**
	 *    Ctor. Creates a new empty imageset into which dynamic icons can be rendered.
	 * @param imagesetName The name of the imageset to create.
	 */
	explicit IconImageStore(std::string imagesetName);

	/**
	 * Ctor. Creates a new imageset from an already existing texture. The whole texture will be used for a single icon.
	 * Use this when you already have an icon.
	 * @param imagesetName The name of the imageset to create.
	 * @param texPtr The texture to use.
	 */
	IconImageStore(const std::string& imagesetName, const Ogre::TexturePtr& texPtr);

	virtual ~IconImageStore();


	/**
	 * Gets the number of icons in this store that haven't been claimed yet.
	 * @return
	 */
	size_t getNumberOfUnclaimedIcons();


	/**
	 * Claims an icon from the store.
	 * @return An entry or null if there are no unclaimed left.
	 */
	IconImageStoreEntry* claimImageEntry();

	/**
	 * Returns an already claimed entry to the store, so that it can be reused by other icons.
	 * @param imageEntry
	 */
	void returnImageEntry(IconImageStoreEntry* imageEntry);

	//Ogre::Image& getImage();

//     Ogre::TexturePtr getTexture();

private:
	void createImageset();

	void createEntries();

	std::string mImagesetName;
	int mIconSize;
	uint32_t mImageSize;
	Ogre::TexturePtr mTexPtr;
	//Ogre::MemoryDataStream* mImageDataStream;
	//Ogre::Image mImage;
	CEGUI::Texture* mCeguiTexture;

	std::vector<std::unique_ptr<IconImageStoreEntry>> mIconImages;
	IconImageStoreEntryStack mUnclaimedIconImages;

};

}


#endif
