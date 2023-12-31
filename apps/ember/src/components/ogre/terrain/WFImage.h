/*
 Copyright (C) 2009 Erik Ogenvik <erik@ogenvik.org>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software Foundation,
 Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#ifndef EMBEROGRETERRAINWFIMAGE_H_
#define EMBEROGRETERRAINWFIMAGE_H_

#include "Image.h"


namespace Ember::OgreView::Terrain {

class WFImage : public Image {
public:
	explicit WFImage(std::unique_ptr<Image::ImageBuffer> buffer);

	void blit(const OgreImage& imageToBlit, unsigned int destinationChannel, int widthOffset, int heightOffset) override;

	void blit(const WFImage& imageToBlit, unsigned int destinationChannel, int widthOffset, int heightOffset) override;
};

}


#endif /* EMBEROGRETERRAINWFIMAGE_H_ */
