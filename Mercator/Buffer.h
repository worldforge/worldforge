// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef MERCATOR_BUFFER_H
#define MERCATOR_BUFFER_H

namespace Mercator {

class Segment;

class Buffer {
  private:
    Segment & m_segment;
    unsigned int m_channels;
    float * const m_data;

  public:
    explicit Buffer(Segment & segment, unsigned int channels = 4);
    virtual ~Buffer();

    Segment & getSegment() const {
        return m_segment;
    }

    unsigned int getChannels() const {
        return m_channels;
    }

    float * getData() {
        return m_data;
    }

};

} // namespace Mercator

#endif // MERCATOR_BUFFER_H
