// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef MERCATOR_MATRIX_H
#define MERCATOR_MATRIX_H

namespace Mercator {

template <int COLS, int ROWS, typename FloatType = float>
class Matrix {
  private:
    FloatType m_data[COLS * ROWS];
  public:
    Matrix() { }

    FloatType & operator()(int col, int row) {
        return m_data[row * COLS + col];
    }

    const FloatType & operator()(int col, int row) const {
        return m_data[row * COLS + col];
    }

    FloatType & operator[](int idx) {
        return m_data[idx];
    }
};

}

#endif // MERCATOR_MATRIX_H
