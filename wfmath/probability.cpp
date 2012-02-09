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

static double LogPoisson(double mean, unsigned int step);
static double IncompleteGamma(double a, double z);
static double IncompleteGammaNoPrefactor(double a, double z);
static double IncompleteGammaComplement(double a, double z);
static double IncompleteGammaComplementNoPrefactor(double a, double z);
// Making this an int makes LogFactorial faster
static const unsigned int GammaCutoff = 10;

double GaussianConditional(double mean, double stddev, double val)
{
  assert(stddev != 0);

  double diff = val - mean;
  double diffnorm = diff / stddev;
  double diffsqr_over_two = diffnorm * diffnorm / 2;

  /* Make sure round off error in Sqrt3 doesn't hit
   * assert() in IncompleteGammaComplementNoPrefactor()
   */
  if(diffnorm < numeric_constants<double>::sqrt3() + 10 * DBL_EPSILON) {
    double erfc_norm = IncompleteGammaComplement(0.5, diffsqr_over_two);

    double normalization = (diffnorm > 0) ? (erfc_norm / 2) : (1 - erfc_norm / 2);

    return Gaussian(mean, stddev, val) / normalization;
  }

  return 2.0 / (fabs(diff)
	 * IncompleteGammaComplementNoPrefactor(0.5, diffsqr_over_two));
}

double Gaussian(double mean, double stddev, double val)
{
  assert(stddev != 0);

  double diff = (mean - val) / stddev;

  return std::exp(-(diff * diff) / 2) / (std::fabs(stddev) *
         (numeric_constants<double>::sqrt_pi() *
          numeric_constants<double>::sqrt2()));
}

double PoissonConditional(double mean, unsigned int step)
{
  assert(mean >= 0);

  if(mean == 0) // Funky limit, but allow it
    return (step == 0) ? 1 : 0;

  if(step == 0)
    return exp(-mean);

  if(mean > step + 1)
    return Poisson(mean, step) / IncompleteGamma(step, mean);

  return 1.0 / IncompleteGammaNoPrefactor(step, mean);
}

double Poisson(double mean, unsigned int step)
{
  assert(mean >= 0);

  if(mean == 0) // Funky limit, but allow it
    return (step == 0) ? 1 : 0;

  return exp(LogPoisson(mean, step));
}

static double LogPoisson(double mean, unsigned int step)
{
  assert(mean > 0);

  if(step == 0)
    return -mean;

  double first = step * log(mean);
  double second = mean +  LogFactorial(step);

  assert("LogFactorial() always returns positive" && second > 0);

  double ans = first - second;

  // can only get probability == 1 for step == mean == 0
  assert("must get probability < 1" && ans < 0);

  return ans;
}

double Factorial(unsigned int n)
{
  if(n == 0 || n == 1)
    return 1;

  if(n < GammaCutoff) {
    double ans = n;
    while(--n > 1) // Don't need to multiply by 1
      ans *= n;
    return ans;
  }
  else
    return exp(LogGamma(n + 1));
}

double LogFactorial(unsigned int n)
{
  if(n == 0 || n == 1)
    return 0; // ln(0!) = ln(1!) = ln(1) = 0

  if(n < GammaCutoff) {
    double ans = n;
    while(--n > 1) // Don't need to multiply by 1
      ans *= n;
    return log(ans);
  }
  else
    return LogGamma(n + 1);
}

double Gamma(double z)
{
  if(z >= 0.5)
    return std::exp(LogGamma(z));
  else
    return numeric_constants<double>::pi() *
           std::exp(-LogGamma(1 - z)) /
           std::sin(numeric_constants<double>::pi() * z);
}

double LogGamma(double z)
{
  /* This will return nan or something when z is a non-positive
   * integer (from trying to take log(0)), but that's what
   * should happen anyway.
   */
  if(z < 0.5)
    return numeric_constants<double>::log_pi() -
           LogGamma(1 - z) -
           std::log(std::fabs(std::sin(numeric_constants<double>::pi() * z)));

  if(z == 0.5) // special case for Gaussian
    return numeric_constants<double>::log_pi() / 2;

  if(z == 1 || z == 2) // 0! and 1!
    return 0;

  double log_shift;

  if(z < GammaCutoff) {
    double shift = 1;
    while(z < GammaCutoff)
      shift *= z++;
    log_shift = log(fabs(shift));
  }
  else
    log_shift = 0;

  // Stirling approximation (see Gradshteyn + Ryzhik, Table of Integrals,
  // Series, and Products, fifth edition, formula 8.344 for a specific formula)

  double ans = (z - 0.5) * std::log(z) - log_shift - z +
               (numeric_constants<double>::log_pi() +
                numeric_constants<double>::log2()) / 2;

  // coeffs[i] is the 2*(i+1)th Bernoulli number, divided by (2i + 1)*(2i + 2)
  const double coeffs[] = 	{1.0/12.0,		-1.0/360.0,
	 1.0/1260.0,		-1.0/1620.0,		 5.0/5940.0,
	-691.0/360360.0,	 7.0/1092.0,		-3617.0/122400.0,
	 43867.0/244188.0,	-174661.0/125400.0,	 854513.0/63756.0,};
  const int num_coeffs = sizeof(coeffs) / sizeof(double);

  double z_power = 1/z;
  double z_to_minus_two = z_power * z_power;
  double small_enough = fabs(ans) * DBL_EPSILON;
  int i;

  for(i = 0; i < num_coeffs; ++i) {
    double next_term = coeffs[i] * z_power;
    ans += next_term;
    if(fabs(next_term) < small_enough)
      break;
    z_power *= z_to_minus_two;
  }

  assert(i < num_coeffs); // If someone hits this, tell me and I'll add more terms,
			  // worst case is for n = cutoff = 10, which should work
			  // for DBL_EPSILON > 1.048e-21

  return ans;
}

static double IncompleteGamma(double a, double z)
{
  assert(z >= 0 && a >= 0); // We only use this part of the parameter space

  if(a == 0)
    return 1;
  else if(z == 0)
    return 0;

  if(z > a + 1)
    return 1 - IncompleteGammaComplement(a, z);

  double prefactor = exp(a * (log(z) + 1) - z - LogGamma(a));

  return IncompleteGammaNoPrefactor(a, z) * prefactor;
}

static double IncompleteGammaNoPrefactor(double a, double z)
{
  assert(z <= a + 1 && z > 0 && a > 0);

  // Power series

  double term = 1;
  double dividend = a;

  double ans = term;
  while(fabs(term / ans) > DBL_EPSILON) {
    term *= z / ++dividend;
    ans += term;
  }

  return ans;
}

static double IncompleteGammaComplement(double a, double z)
{
  assert(z >= 0 && a >= 0); // We only use this part of the parameter space

  if(a == 0)
    return 0;
  else if(z == 0)
    return 1;

  if(z < a + 1)
    return 1 - IncompleteGamma(a, z);

  double prefactor = exp(a * log(z) - z - LogGamma(a));

  return IncompleteGammaComplementNoPrefactor(a, z) * prefactor;
}

static double IncompleteGammaComplementNoPrefactor(double a, double z)
{
  assert(z >= a + 1 && a > 0);

  // Continued fraction

  const double fudge = 1000;

  double b_contrib = z + 1 - a;
  double a_last, b_last, a_next, b_next;
  int term = 1;
  bool last_zero, next_zero = (fabs(b_contrib) <= DBL_MIN * fudge);

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
    double a_tmp = a_next, b_tmp = b_next;

    double a_contrib = term * (a - term);
    ++term;
    b_contrib += 2;

    a_next = b_contrib * a_tmp + a_contrib * a_last;
    b_next = b_contrib * b_tmp + a_contrib * b_last;

    a_last = a_tmp;
    b_last = b_tmp;

    last_zero = next_zero;
    next_zero = (fabs(b_next) <= fabs(a_next) * (DBL_MIN * fudge));

    if(next_zero)
      continue; // b_next is about zero

    a_next /= b_next;

    if(!last_zero && fabs(a_next - a_last) < fabs(a_last) * DBL_EPSILON)
      return a_next; // Can't compare if b_last was zero

    a_last /= b_next;
    b_last /= b_next;
    b_next = 1;
  }
}

}
