// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Damien McGinnes, Ron Steinke, Alistair Riddoch

#ifndef MERCATOR_RANDCACHE_H
#define MERCATOR_RANDCACHE_H

#include <vector>
#include <algorithm>
#include <cstdlib>
#include <wfmath/MersenneTwister.h>

// construct with something like:
// RandCache r(seed, new MyOrdering(args));
// where MyOrdering is derived from RandCache::Ordering.

/// \brief A cache of random values.
class RandCache
{
 public:
  /// Unsigned 32bit integer
  typedef WFMath::MTRand::uint32 uint32;
  /// Size type of std::vector.
  typedef std::vector<uint32>::size_type size_type;

  /// \brief Interface to define the ordering of the random number cache.
  struct Ordering {
    virtual ~Ordering() {}
    /// \brief Determine the order.
    virtual size_type operator()(int x, int y) = 0;
  };

  /// \brief Constructor
  ///
  /// @param seed the random seed value for generated numbers.
  /// @param o the ordering object that defines the sequence generated.
  RandCache(uint32 seed, Ordering* o) :
        m_rand(seed), m_ordering(o) {}
  /// \brief Constructor
  ///
  /// @param seed the random seed block for generated numbers.
  /// @param seed_len the length of the seed block.
  /// @param o the ordering object that defines the sequence generated.
  RandCache(uint32* seed, uint32 seed_len, Ordering* o) :
        m_rand(seed, seed_len), m_ordering(o) {}
  ~RandCache() {delete m_ordering;}

  /// \brief Retrieve a random value associated with parameters
  ///
  /// @param x coordinate associated with value to be retrieved.
  /// @param y coordinate associated with value to be retrieved.
  double operator()(int x, int y)
  {
    size_type cache_order = (*m_ordering)(x, y);

    // make sure we've cached the value
    if(cache_order >= m_cache.size()) {
      size_type old_size = m_cache.size();
      m_cache.resize(cache_order + 64); //do 64 at once
      while(old_size < m_cache.size())
        m_cache[old_size++] = m_rand.randInt();
    }

    return double(m_cache[cache_order] * (1.0/4294967295.0));
  }

 private:
  /// \brief Source random number generator.
  WFMath::MTRand m_rand;
  /// \brief Store for the cache of values.
  std::vector<uint32> m_cache;
  /// \brief Ordering object that defines the ordering of the cache.
  Ordering* m_ordering;
};

/// \brief A spiral around 0,0
class ZeroSpiralOrdering : public RandCache::Ordering
{
public:
    RandCache::size_type operator () (int x, int y) 
    {
        if (x==0 && y==0) return 0;
        
        int d=std::max(std::abs(x), std::abs(y));
        int min=(2*d-1)*(2*d-1);

        if (y == d)  return min + 2*d - x;
        if (x == -d) return min + 4*d - y;
        if (y == -d) return min + 6*d + x;
        else { //if (x == d) {
            if (y >=0) return min + y;
            else return min + 8*d + y;
        }
    }
};

/// \brief A spiral around x,y
class SpiralOrdering : public ZeroSpiralOrdering
{
private:
    /// The centre x coordinate of the spiral.
    int m_x;
    /// The centre y coordinate of the spiral.
    int m_y;
public:
    /// \brief Constructor
    ///
    /// @param x centre x coordinate of the spiral.
    /// @param y centre y coordinate of the spiral.
    SpiralOrdering(int x, int y) : ZeroSpiralOrdering(), m_x(x), m_y(y) {}
    RandCache::size_type operator () (int x, int y) 
    {
        return ZeroSpiralOrdering::operator()(x-m_x, y-m_y);
    }
};


#endif



