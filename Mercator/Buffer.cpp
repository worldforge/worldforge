// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include <Mercator/Buffer.h>

#include <Mercator/Segment.h>

namespace Mercator {

Buffer::Buffer(Segment & segment, unsigned int channels) :
         m_segment(segment), m_channels(channels),
         m_data(new float[segment.getSize() * segment.getSize() * channels])
{
}

Buffer::~Buffer()
{
    delete [] m_data;
}

} // namespace Mercator
