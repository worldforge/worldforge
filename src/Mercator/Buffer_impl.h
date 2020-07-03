// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include "Buffer.h"

namespace Mercator {

template <typename DataType>
Buffer<DataType>::Buffer(unsigned int size, unsigned int channels) :
         m_channels(channels), m_size(size)
{
}

template <typename DataType>
Buffer<DataType>::~Buffer() = default;

} // namespace Mercator
