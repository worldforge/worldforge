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
  typedef std::vector<double>::size_type size_type;
  typedef WFMath::MTRand::uint32 uint32;

  struct Ordering {
    virtual ~Ordering() {}
    virtual size_type operator()(int x, int y) = 0;
  };

  RandCache(uint32 seed, Ordering* o) :
	rand_(seed), ordering_(o) {}
  RandCache(uint32* seed, uint32 seed_len, Ordering* o) :
	rand_(seed, seed_len), ordering_(o) {}
  ~RandCache() {delete ordering_;}

  double operator()(int x, int y)
  {
    size_type cache_order = (*ordering_)(x, y);

    // make sure we've cached the value
    if(cache_order >= cache_.size()) {
      size_type old_size = cache_.size();
      cache_.resize(cache_order + 64); //do 64 at once
      while(old_size < cache_.size())
        cache_[old_size++] = rand_();
    }

    return cache_[cache_order];
  }

 private:
  WFMath::MTRand rand_;
  std::vector<double> cache_;
  Ordering* ordering_;
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


