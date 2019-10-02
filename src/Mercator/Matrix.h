// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef MERCATOR_MATRIX_H
#define MERCATOR_MATRIX_H

namespace Mercator {

/// \brief A fixed sized array of objects.
///
/// Mainly used to store the control points in a Segment.
template <unsigned int COLS, unsigned int ROWS, typename FloatType = float>
class Matrix {
  private:
    /// \brief Storage for the array of objects.
    FloatType m_data[COLS * ROWS];
  public:
    /// \brief Constructor for the Matrix.
    Matrix() { }

    /// \brief Accessor for modifying the array.
    FloatType & operator()(unsigned int col, unsigned int row) {
        return m_data[row * COLS + col];
    }

    /// \brief Accessor for the array.
    const FloatType & operator()(unsigned int col, unsigned int row) const {
        return m_data[row * COLS + col];
    }

    /// \brief Accessor for accessing the array as one dimensional.
    FloatType & operator[](unsigned int idx) {
        return m_data[idx];
    }
};

}

#endif // MERCATOR_MATRIX_H
