// -*-C++-*-
// probability_test.cpp (probability and statistics test functions)
//
//  The WorldForge Project
//  Copyright (C) 2001  The WorldForge Project
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//  For information about WorldForge and its authors, please contact
//  the Worldforge Web Site at http://www.worldforge.org.

// Author: Ron Steinke
// Created: 2002-1-24

#include "const.h"
#include "probability.h"
#include <iostream>

using namespace WFMath;

const double fudge = 3000;
const double use_epsilon = fudge * DBL_EPSILON;

void test_probability(double mean, double std_dev, double step)
{
  assert(step > 0);
  assert(mean > 0);

  double one_minus_gauss_sum = 0.5, poisson_sum = 0;
  bool gauss_done = false, poisson_done = false, past_poisson_peak = false;
//  double my_fudge = 1;

  for(unsigned int num_step = 0; !gauss_done || !poisson_done; ++num_step) {
//    cout << "Step " << num_step << std::endl;
    if(!gauss_done) {
      double val = Gaussian(mean, std_dev, mean + step * num_step);
//      cout << "val = " << val << std::endl;
      double cond = GaussianConditional(mean, std_dev, mean + step * num_step);
//      cout << "cond = " << cond << std::endl;
      double cond_conj = GaussianConditional(mean, std_dev, mean - step * num_step);
//      cout << "cond_conj = " << cond_conj << std::endl;

      if(val > fudge * DBL_MIN) {
        if(cond_conj != 0) {
          double first_frac = val / cond, second_frac = val / cond_conj;
          double sum = first_frac + second_frac;
//          cout << first_frac << ',' << second_frac << ',' << sum - 1 << std::endl;
          assert(IsFloatEqual(sum, 1, use_epsilon));
//          while(!IsFloatEqual(sum, 1, my_fudge * DBL_EPSILON))
//            my_fudge *= 1.1;
        }

        if(num_step != 0) {
//          cout << "one_minus_gauss_sum = " << one_minus_gauss_sum << std::endl;
          assert(IsFloatEqual(val, one_minus_gauss_sum * cond, fudge * step / mean));
          one_minus_gauss_sum = FloatSubtract(val / cond, val * step, use_epsilon);
          assert(one_minus_gauss_sum > -use_epsilon); // Avoid cumulative roundoff errors
        }
      }
      else
        gauss_done = true;
    }
    if(!poisson_done) {
      double val = Poisson(mean / step, num_step);
//      cout << "val = " << val << std::endl;
      double cond = PoissonConditional(mean / step, num_step);
//      cout << "cond = " << cond << std::endl;
//      cout << "poisson_sum = " << poisson_sum << std::endl;

      assert(val <= 1 + use_epsilon);
      assert(cond <= 1 + use_epsilon);
      assert(IsFloatEqual(val + poisson_sum * cond, cond, use_epsilon));

      poisson_sum += val;
      assert(poisson_sum <= 1 + use_epsilon);

      if(val > fudge * DBL_MIN && past_poisson_peak)
        poisson_done = true;
      else if(val > DBL_EPSILON)
        past_poisson_peak = true;
    }
  }
//  cout << "my_fudge = " << my_fudge << std::endl;
}

int main()
{
  test_probability(2.0, 0.5, 0.01);

  test_probability(0.3, 2.0, 0.01);

  return 0;
}
