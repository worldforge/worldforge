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

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "const.h"
#include "probability.h"
#include "shuffle.h"
#include <iostream>
#include <vector>

#include <cassert>

using namespace WFMath;

const double fudge = 3000;
const double use_epsilon = fudge * std::numeric_limits<double>::epsilon();

void test_probability(double mean, double std_dev, double step)
{
  assert(step > 0);
  assert(mean > 0);

  double one_minus_gauss_sum = 0.5, poisson_sum = 0;
  bool gauss_done = false, poisson_done = false, past_poisson_peak = false;
//  double my_fudge = 1;

  for(unsigned int num_step = 0; !gauss_done || !poisson_done; ++num_step) {
//    std::cout << "Step " << num_step << std::endl;
    if(!gauss_done) {
      double val = Gaussian(mean, std_dev, mean + step * num_step);
//      std::cout << "val = " << val << std::endl;
      double cond = GaussianConditional(mean, std_dev, mean + step * num_step);
//      std::cout << "cond = " << cond << std::endl;
      double cond_conj = GaussianConditional(mean, std_dev, mean - step * num_step);
//      std::cout << "cond_conj = " << cond_conj << std::endl;

      if(val > fudge * std::numeric_limits<double>::min()) {
        if(cond_conj != 0) {
//          double first_frac = val / cond, second_frac = val / cond_conj;
//          double sum = first_frac + second_frac;
//          std::cout << first_frac << ',' << second_frac << ',' << sum - 1 << std::endl;
//          assert(Equal(sum, 1, use_epsilon));
//          while(!Equal(sum, 1, my_fudge * std::numeric_limits<double>::epsilon()))
//            my_fudge *= 1.1;
        }

        if(num_step != 0) {
//          std::cout << "one_minus_gauss_sum = " << one_minus_gauss_sum << std::endl;
          assert(Equal(val, one_minus_gauss_sum * cond, fudge * step / mean));
          one_minus_gauss_sum = val / cond - val * step;
          assert(one_minus_gauss_sum > -use_epsilon); // Avoid cumulative roundoff errors
        }
      }
      else
        gauss_done = true;
    }
    if(!poisson_done) {
      double val = Poisson(mean / step, num_step);
//      std::cout << "val = " << val << std::endl;
      double cond = PoissonConditional(mean / step, num_step);
//      std::cout << "cond = " << cond << std::endl;
//      std::cout << "poisson_sum = " << poisson_sum << std::endl;

      assert(val <= 1 + use_epsilon);
      assert(cond <= 1 + use_epsilon);
      assert(Equal(val + poisson_sum * cond, cond, use_epsilon));

      poisson_sum += val;
      assert(poisson_sum <= 1 + use_epsilon);

      if(val > fudge * std::numeric_limits<double>::min() && past_poisson_peak)
        poisson_done = true;
      else if(val > std::numeric_limits<double>::epsilon())
        past_poisson_peak = true;
    }
  }
//  std::cout << "my_fudge = " << my_fudge << std::endl;
}

// just a simple test, to make sure it's not really broken
void test_shuffle()
{
  const int nums[] = {0, 4, 1, -5, 7, 3};
  unsigned num_nums = sizeof(nums)/sizeof(nums[0]);
  std::vector<int> foo(nums, nums + num_nums);
  Shuffle(foo);
//  for(unsigned i = 0; i < foo.size(); ++i)
//    std::cerr << foo[i] << ' ';
//  std::cerr << std::endl;
}

int main()
{
  test_probability(2.0, 0.5, 0.001);

  test_probability(0.3, 2.0, 0.001);

  return 0;
}
