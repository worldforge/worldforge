// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Damien McGinnes, Ron Steinke, Alistair Riddoch

#ifndef MERCATOR_RANDCACHE_H
#define MERCATOR_RANDCACHE_H

#include <vector>
#include <wfmath/MersenneTwister.h>

// construct with something like:
// RandCache r(seed, new MyOrdering(args));
// where MyOrdering is derived from RandCache::Ordering.

class RandCache
{
 public:
  typedef WFMath::MTRand::uint32 uint32;
  typedef std::vector<uint32>::size_type size_type;

  struct Ordering {
    virtual ~Ordering() {}
    virtual size_type operator()(int x, int y) = 0;
  };

  RandCache(uint32 seed, Ordering* o) :
	m_rand(seed), m_ordering(o) {}
  RandCache(uint32* seed, uint32 seed_len, Ordering* o) :
	m_rand(seed, seed_len), m_ordering(o) {}
  ~RandCache() {delete m_ordering;}

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
  WFMath::MTRand m_rand;
  std::vector<uint32> m_cache;
  Ordering* m_ordering;
};

//a spiral around 0,0
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

//A spiral around x,y
class SpiralOrdering : public ZeroSpiralOrdering
{
private:
    int m_x, m_y;
public:
    SpiralOrdering(int x, int y) : ZeroSpiralOrdering(), m_x(x), m_y(y) {}
    RandCache::size_type operator () (int x, int y) 
    {
        return ZeroSpiralOrdering::operator()(x-m_x, y-m_y);
    }
};


#endif



