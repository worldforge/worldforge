// probability.h (probability and statistics functions)
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

// These functions will use double instead of CoordType, for accuracy

#ifndef WFMATH_PROBABILTIY_H
#define WFMATH_PROBABILTIY_H

namespace WFMath {

/// Gives the conditional probability of the Gaussian distribution at position val
/**
 * The probability that a Gaussian random variable will fall between
 * val and val + delta, given that it is already known to be not
 * less than val, is given by this function multiplied by delta
 * (for small delta).
 **/
double GaussianConditional(double mean, double stddev, double val);
/// Gives the value of the Gaussian distribution at position val
double Gaussian(double mean, double stddev, double val);

/// Gives the conditional probability of the Poisson distribution at position step
/**
 * Returns the probability that a Poisson random variable will have value
 * step, given that it is already known not to be less than step.
 **/
double PoissonConditional(double mean, unsigned int step);
/// Gives the value of the Poisson distribution at position step
double Poisson(double mean, unsigned int step);

/// Gives the natural log of n!
double LogFactorial(unsigned int n);
/// Gives n!
double Factorial(unsigned int n);

/// The natural log of Euler's Gamma function
double LogGamma(double z);
/// Euler's Gamma function
double Gamma(double z);

} // namespace WFMath

#endif  // WFMATH_PROBABILITY_H
