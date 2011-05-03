

//    Copright (C) 1999
//    $Revision$
//    $Date$
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA,
//    or download the License terms from prep.ai.mit.edu/pub/gnu/COPYING-2.0.
//
//    Contact:
//    --------
//    Bernd Gaertner
//    Institut f. Informatik
//    ETH Zuerich
//    ETH-Zentrum
//    CH-8092 Zuerich, Switzerland
//    http://www.inf.ethz.ch/personal/gaertner
//

// 2001-1-9: included in WFMath backend. Namespace wrapping added
// and filename changed to follow WFMath conventions, but otherwise
// unchanged.

#ifndef WFMATH_MINIBALL_H
#define WFMATH_MINIBALL_H

#include <list>
#include <wfmath/wrapped_array.h>

namespace WFMath { namespace _miniball {

    template <int d> class Miniball;
    template <int d> class Basis;

    // Miniball
    // --------
    
    template <int d>
    class Miniball {
        public:
            // types
            typedef Wrapped_array<d>                            Point;
            typedef typename std::list<Point>::iterator         It;
            typedef typename std::list<Point>::const_iterator   Cit;
    
        private:
            // data members
            std::list<Point> L;         // STL list keeping the points
            Basis<d>    B;              // basis keeping the current ball
            It          support_end;    // past-the-end iterator of support set
    
            // private methods
            void        mtf_mb (It k);
            void        pivot_mb (It k);
            void        move_to_front (It j);
            double      max_excess (It t, It i, It& pivot) const;
            double      abs (double r) const {return (r>0)? r: (-r);}
            double      sqr (double r) const {return r*r;}
    
        public:
            // construction
            Miniball() : L(), B(), support_end() {}
            void        check_in (const Point& p);
            void        build (bool pivoting = true);
    
            // access
            Point       center() const;
            double      squared_radius () const;
            int         nr_points () const;
            Cit         points_begin () const;
            Cit         points_end () const;
            int         nr_support_points () const;
            Cit         support_points_begin () const;
            Cit         support_points_end () const;
    
            // checking
            double      accuracy (double& slack) const;
            bool        is_valid (double tolerance = 1e-15) const;
     };
    
    

    // Basis
    // -----
    
    template <int d>
    class Basis {
        private:
            // types
            typedef Wrapped_array<d>            Point;
    
            // data members
            int                 m, s;   // size and number of support points
            double              q0[d];
    
            double              z[d+1];
            double              f[d+1];
            double              v[d+1][d];
            double              a[d+1][d];
    
            double              c[d+1][d];
            double              sqr_r[d+1];
    
            double*             current_c;      // points to some c[j]
            double              current_sqr_r;
    
            double              sqr (double r) const {return r*r;}
    
        public:
            Basis();
    
            // access
            const double*       center() const;
            double              squared_radius() const;
            int                 size() const;
            int                 support_size() const;
            double              excess (const Point& p) const;
    
            // modification
            void                reset(); // generates empty sphere with m=s=0
            bool                push (const Point& p);
            void                pop ();
    
            // checking
            double              slack() const;
    };
    
}} // namespace WFMath::_miniball   

#endif // WFMATH_MINIBALL_H
