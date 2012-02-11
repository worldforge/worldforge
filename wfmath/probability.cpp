// probability.cpp (probability and statistics implementation)
//
//  The WorldForge Project
//  Copyright (C) 2002  The WorldForge Project
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
// Created: 2002-1-23

#include <wfmath/probability.h>

#include <wfmath/const.h>

#include <cmath>

#include <cassert>

namespace WFMath {

template<typename FloatT>
static FloatT LogPoisson(FloatT mean, unsigned int step);
template<typename FloatT>
static FloatT IncompleteGamma(FloatT a, FloatT z);
template<typename FloatT>
static FloatT IncompleteGammaNoPrefactor(FloatT a, FloatT z);
template<typename FloatT>
static FloatT IncompleteGammaComplement(FloatT a, FloatT z);
template<typename FloatT>
static FloatT IncompleteGammaComplementNoPrefactor(FloatT a, FloatT z);

// Making this an int makes LogFactorial faster
static const unsigned int GammaCutoff = 10;

template<typename FloatT>
FloatT GaussianConditional(FloatT mean, FloatT stddev, FloatT val)
{
  assert(stddev != 0);

  FloatT diff = val - mean;
  FloatT diffnorm = diff / stddev;
  FloatT diffsqr_over_two = diffnorm * diffnorm / 2;

  /* Make sure round off error in Sqrt3 doesn't hit
   * assert() in IncompleteGammaComplementNoPrefactor()
   */
  static const FloatT half = 0.5;
  if(diffnorm < numeric_constants<FloatT>::sqrt3() +
                10 * std::numeric_limits<FloatT>::epsilon()) {
    FloatT erfc_norm = IncompleteGammaComplement(half, diffsqr_over_two);

    FloatT normalization = (diffnorm > 0) ? (erfc_norm / 2) : (1 - erfc_norm / 2);

    return Gaussian(mean, stddev, val) / normalization;
  }
  static const FloatT two = 2.0;

  return two / (std::fabs(diff)
	 * IncompleteGammaComplementNoPrefactor<FloatT>(half, diffsqr_over_two));
}

template<typename FloatT>
FloatT Gaussian(FloatT mean, FloatT stddev, FloatT val)
{
  assert(stddev != 0);

  FloatT diff = (mean - val) / stddev;

  return std::exp(-(diff * diff) / 2) / (std::fabs(stddev) *
         (numeric_constants<FloatT>::sqrt_pi() *
          numeric_constants<FloatT>::sqrt2()));
}

template<typename FloatT>
FloatT PoissonConditional(FloatT mean, unsigned int step)
{
  assert(mean >= 0);

  if(mean == 0) // Funky limit, but allow it
    return (step == 0) ? 1 : 0;

  if(step == 0)
    return std::exp(-mean);

  if(mean > step + 1)
    return Poisson(mean, step) / IncompleteGamma<FloatT>(step, mean);

  return 1.0 / IncompleteGammaNoPrefactor<FloatT>(step, mean);
}

template<typename FloatT>
FloatT Poisson(FloatT mean, unsigned int step)
{
  assert(mean >= 0);

  if(mean == 0) // Funky limit, but allow it
    return (step == 0) ? 1 : 0;

  return std::exp(LogPoisson(mean, step));
}

template<typename FloatT>
static FloatT LogPoisson(FloatT mean, unsigned int step)
{
  assert(mean > 0);

  if(step == 0)
    return -mean;

  FloatT first = step * std::log(mean);
  FloatT second = mean +  LogFactorial<FloatT>(step);

  assert("LogFactorial() always returns positive" && second > 0);

  FloatT ans = first - second;

  // can only get probability == 1 for step == mean == 0
  assert("must get probability < 1" && ans < 0);

  return ans;
}

template<typename FloatT>
FloatT Factorial(unsigned int n)
{
  if(n == 0 || n == 1)
    return 1;

  if(n < GammaCutoff) {
    FloatT ans = n;
    while(--n > 1) // Don't need to multiply by 1
      ans *= n;
    return ans;
  }
  else
    return std::exp(LogGamma<FloatT>(n + 1));
}

template<typename FloatT>
FloatT LogFactorial(unsigned int n)
{
  if(n == 0 || n == 1)
    return 0; // ln(0!) = ln(1!) = ln(1) = 0

  if(n < GammaCutoff) {
    FloatT ans = n;
    while(--n > 1) // Don't need to multiply by 1
      ans *= n;
    return std::log(ans);
  }
  else
    return LogGamma<FloatT>(n + 1);
}

template<typename FloatT>
FloatT Gamma(FloatT z)
{
  if(z >= 0.5)
    return std::exp(LogGamma(z));
  else
    return numeric_constants<FloatT>::pi() *
           std::exp(-LogGamma(1 - z)) /
           std::sin(numeric_constants<FloatT>::pi() * z);
}

template<typename FloatT>
FloatT LogGamma(FloatT z)
{
  /* This will return nan or something when z is a non-positive
   * integer (from trying to take log(0)), but that's what
   * should happen anyway.
   */
  static const FloatT one = 1.0;

  if(z < 0.5)
    return numeric_constants<FloatT>::log_pi() -
           LogGamma<FloatT>(one - z) -
           std::log(std::fabs(std::sin(numeric_constants<FloatT>::pi() * z)));

  if(z == 0.5) // special case for Gaussian
    return numeric_constants<FloatT>::log_pi() / 2;

  if(z == 1 || z == 2) // 0! and 1!
    return 0;

  FloatT log_shift;

  if(z < GammaCutoff) {
    FloatT shift = 1;
    while(z < GammaCutoff)
      shift *= z++;
    log_shift = std::log(std::fabs(shift));
  }
  else
    log_shift = 0;

  // Stirling approximation (see Gradshteyn + Ryzhik, Table of Integrals,
  // Series, and Products, fifth edition, formula 8.344 for a specific formula)

  static const FloatT half = 0.5, two = 2.0;

  FloatT ans = (z - half) * std::log(z) - log_shift - z +
               (numeric_constants<FloatT>::log_pi() +
                numeric_constants<FloatT>::log2()) / two;

  // coeffs[i] is the 2*(i+1)th Bernoulli number, divided by (2i + 1)*(2i + 2)
  static const FloatT coeffs[] = 	{1.0/12.0,		-1.0/360.0,
	 1.0/1260.0,		-1.0/1620.0,		 5.0/5940.0,
	-691.0/360360.0,	 7.0/1092.0,		-3617.0/122400.0,
	 43867.0/244188.0,	-174661.0/125400.0,	 854513.0/63756.0,};
  static const int num_coeffs = sizeof(coeffs) / sizeof(FloatT);

  FloatT z_power = 1/z;
  FloatT z_to_minus_two = z_power * z_power;
  FloatT small_enough = std::fabs(ans) * std::numeric_limits<FloatT>::epsilon();
  int i;

  for(i = 0; i < num_coeffs; ++i) {
    FloatT next_term = coeffs[i] * z_power;
    ans += next_term;
    if(std::fabs(next_term) < small_enough)
      break;
    z_power *= z_to_minus_two;
  }

  assert(i < num_coeffs); // If someone hits this, tell me and I'll add more terms,
			  // worst case is for n = cutoff = 10, which should work
			  // for DBL_EPSILON > 1.048e-21

  return ans;
}

template<typename FloatT>
static FloatT IncompleteGamma(FloatT a, FloatT z)
{
  assert(z >= 0 && a >= 0); // We only use this part of the parameter space

  if(a == 0)
    return 1;
  else if(z == 0)
    return 0;

  if(z > a + 1)
    return 1 - IncompleteGammaComplement(a, z);

  FloatT prefactor = std::exp(a * (std::log(z) + 1) - z - LogGamma(a));

  return IncompleteGammaNoPrefactor(a, z) * prefactor;
}

template<typename FloatT>
static FloatT IncompleteGammaNoPrefactor(FloatT a, FloatT z)
{
  assert(z <= a + 1 && z > 0 && a > 0);

  // Power series

  FloatT term = 1;
  FloatT dividend = a;

  FloatT ans = term;
  while(std::fabs(term / ans) > std::numeric_limits<FloatT>::epsilon()) {
    term *= z / ++dividend;
    ans += term;
  }

  return ans;
}

template<typename FloatT>
static FloatT IncompleteGammaComplement(FloatT a, FloatT z)
{
  assert(z >= 0 && a >= 0); // We only use this part of the parameter space

  if(a == 0)
    return 0;
  else if(z == 0)
    return 1;

  if(z < a + 1)
    return 1 - IncompleteGamma(a, z);

  FloatT prefactor = std::exp(a * std::log(z) - z - LogGamma(a));

  return IncompleteGammaComplementNoPrefactor(a, z) * prefactor;
}

template<typename FloatT>
static FloatT IncompleteGammaComplementNoPrefactor(FloatT a, FloatT z)
{
  assert(z >= a + 1 && a > 0);

  // Continued fraction

  static const FloatT fudge = 1000;

  FloatT b_contrib = z + 1 - a;
  FloatT a_last, b_last, a_next, b_next;
  FloatT term = 1;
  bool last_zero,
       next_zero = (std::fabs(b_contrib) <= std::numeric_limits<FloatT>::min()
                                            * fudge);

  if(next_zero) {
    a_last = 0;
    b_last = 1;
    a_next = 1;
    b_next = b_contrib;
  }
  else {
    a_last = 0;
    b_last = 1 / b_contrib;
    a_next = b_last;
    b_next = 1;
  }

  while(true) {
    FloatT a_tmp = a_next, b_tmp = b_next;

    FloatT a_contrib = term * (a - term);
    ++term;
    b_contrib += 2;

    a_next = b_contrib * a_tmp + a_contrib * a_last;
    b_next = b_contrib * b_tmp + a_contrib * b_last;

    a_last = a_tmp;
    b_last = b_tmp;

    last_zero = next_zero;
    next_zero = (std::fabs(b_next) <=
                 std::fabs(a_next) * (std::numeric_limits<FloatT>::min() *
                                      fudge));

    if(next_zero)
      continue; // b_next is about zero

    a_next /= b_next;

    if(!last_zero &&
       std::fabs(a_next - a_last) < std::fabs(a_last) *
                                    std::numeric_limits<FloatT>::epsilon())
      return a_next; // Can't compare if b_last was zero

    a_last /= b_next;
    b_last /= b_next;
    b_next = 1;
  }
}

template
float GaussianConditional<float>(float mean, float stddev, float val);
template
float Gaussian<float>(float mean, float stddev, float val);
template
float PoissonConditional<float>(float mean, unsigned int step);
template
float Poisson<float>(float mean, unsigned int step);
template
float LogFactorial<float>(unsigned int n);
template
float Factorial<float>(unsigned int n);
template
float LogGamma<float>(float z);
template
float Gamma<float>(float z);

template
double GaussianConditional<double>(double mean, double stddev, double val);
template
double Gaussian<double>(double mean, double stddev, double val);
template
double PoissonConditional<double>(double mean, unsigned int step);
template
double Poisson<double>(double mean, unsigned int step);
template
double LogFactorial<double>(unsigned int n);
template
double Factorial<double>(unsigned int n);
template
double LogGamma<double>(double z);
template
double Gamma<double>(double z);

} // namespace WFMath
