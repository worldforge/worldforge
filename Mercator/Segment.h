// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef MERCATOR_SEGMENT_H
#define MERCATOR_SEGMENT_H

namespace Mercator {

// This class will need to be reference counted if we want the code to
// be able to hold onto it, as currently they get deleted internally
// whenever height points are asserted.

class Segment {
  private:
    int m_res;
    double * const m_points;
  public:
    explicit Segment(int res = 64);

    const int getSize() const {
        return m_res;
    }
    
    const double * getPoints() const {
        return m_points;
    }
    double * getPoints() {
        return m_points;
    }

    double get(int x, int y) {
        return m_points[y * m_res + x];
    }

    void populate(double, double, double, double);

};

} // namespace Mercator

#endif // MERCATOR_SEGMENT_H
