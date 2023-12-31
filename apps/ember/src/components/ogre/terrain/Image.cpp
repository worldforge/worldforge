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

#include "Image.h"


namespace Ember::OgreView::Terrain {

Image::Image(std::unique_ptr<ImageBuffer> buffer) :
		mBuffer(std::move(buffer)) {
}

Image::~Image() = default;

unsigned char* Image::getData() {
	return mBuffer->getData();
}

const unsigned char* Image::getData() const {
	return mBuffer->getData();

}

unsigned int Image::getChannels() const {
	return mBuffer->getChannels();
}

size_t Image::getSize() const {
	return mBuffer->getSize();
}

void Image::reset() {
	mBuffer->reset();
}

unsigned int Image::getResolution() const {
	return mBuffer->getResolution();
}

}



