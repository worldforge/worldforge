// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef MERCATOR_BUFFER_H
#define MERCATOR_BUFFER_H

#include <cassert>

namespace Mercator {

class Segment;

/// \brief Template for managing buffers of data for a segment.
template<typename DataType>
class Buffer {
  public:
    const Segment & m_segment;
  private:
    const unsigned int m_channels;
    const unsigned int m_size;
    DataType * m_data;

  public:
    explicit Buffer(const Segment & segment, unsigned int channels = 4);
    virtual ~Buffer();

    DataType & operator()(unsigned int x,unsigned int y,unsigned int channel) {
        assert(m_data != 0);
        return m_data[(y * m_size + x) * m_channels + channel];
    }

    const DataType & operator()(unsigned int x,
                                unsigned int y,
                                unsigned int channel) const {
        assert(m_data != 0);
        return m_data[(y * m_size + x) * m_channels + channel];
    }
    
    const Segment & getSegment() const {
        return m_segment;
    }

    unsigned int getSize() const {
        return m_size;
    }

    unsigned int getChannels() const {
        return m_channels;
    }

    DataType * getData() {
        return m_data;
    }

    void allocate() {
        assert(m_data == 0);
        m_data = new DataType[m_size * m_size * m_channels];
    }

    bool isValid() const {
        return (m_data != 0);
    }

    void invalidate() {
        delete [] m_data;
        m_data = 0;
    }

};

} // namespace Mercator

#endif // MERCATOR_BUFFER_H
