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
    const unsigned int m_channels;
    const unsigned int m_size;
    float * const m_data;

  public:
    explicit Buffer(Segment & segment, unsigned int channels = 4);
    virtual ~Buffer();

    float & operator()(unsigned int x, unsigned int y, unsigned int channel) {
        return m_data[(y * m_size + x) * m_channels + channel];
    }

    const float & operator()(unsigned int x,
                             unsigned int y,
                             unsigned int channel) const {
        return m_data[(y * m_size + x) * m_channels + channel];
    }
    
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
