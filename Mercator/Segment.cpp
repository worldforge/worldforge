// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include <Mercator/Segment.h>
#include <Mercator/Terrain.h>
#include <iostream>
#include <cmath>

namespace Mercator {

static float FALLOFF = 0.15;
static float ROUGHNESS = 1.5;

Segment::Segment(int res) : m_res(res), m_points(new float[(res+1) * (res+1)]),
                            m_max(0.f), m_min(0.0f)
{
}

Segment::~Segment()
{
    delete m_points;
}

void Segment::populate(const Matrix<4, 4> & base)
{
    fill2d(m_res, FALLOFF, ROUGHNESS, base(1, 1), base(2, 1),
                                    base(2, 2), base(1, 2));
}

//rand num between -0.5...0.5
inline float randHalf() {
	return (float) rand() / RAND_MAX - 0.5f;
}


//quasi-Random Midpoint Displacement algorithm
float Segment::qRMD(float nn, float fn, float ff, float nf, float roughness, float falloff, int depth) const {
    float heightDifference = std::max(std::max(nn, fn), std::max(nf, ff)) -
                             std::min(std::min(nn, fn), std::min(nf, ff));
 
    return ((nn+fn+ff+nf)/4.f) + randHalf() * roughness * heightDifference / (1.f+::pow(depth,falloff));
}

//1 dimensional midpoint displacement fractal
//size must be a power of 2
//falloff is the decay of displacement as the fractal is refined
//array is size+1 long. array[0] and array[size] are filled
//      with the control points for the fractal
void Segment::fill1d(int size, float falloff, float roughness, float l, float h, float *array) const {
    assert(falloff!=-1.0);

    array[0] = l;
    array[size] = h;
 
   
    //seed the RNG.
    //The RNG is seeded only once for the line and the seed is based on the
    //two endpoints -because they are the common parameters for two adjoining tiles
    //if extra parameters are added to a tile, the edges need to be common
    //FIXME work out a decent seed
    srand((unsigned int)(l*h*10.232313));

    //stride is used to step across the array in a deterministic fashion
    //effectively we do the 1/2  point, then the 1/4 points, then the 1/8th points etc.
    //this has to be the same order every time because we call on the RNG at every point 
    int stride = size/2;

    //depth is used to indicate what level we are on. the displacement is reduced
    //each time we traverse the array. because this is really the edge of a 2d array, the
    //depth decrement is doubled within the loop
    int depth=1;
 
    while (stride) {
        for (int i=stride;i<size;i+=stride*2) {
	    float hh = array[i-stride];
	    float lh = array[i+stride];
            float hd = fabs(hh-lh); //fabs necessary?
 
            array[i] = ((hh+lh)/2.f) + randHalf() * roughness  * hd / (1+::pow(depth,falloff));
	}
        stride >>= 1;
	depth+=2;
     }
}

//2 dimensional midpoint displacement fractal for a tile where
//edges are to be filled by 1d fractals.
//size must be a power of 2
//array is size+1  * size+1 with the corners the control points.
void Segment::fill2d(int size, float falloff, float roughness, 
		   float p1, float p2, float p3, float p4) {
    assert(falloff!=-1.0);
    assert(m_points!=NULL);
    
    int line = size+1;
    
    //calculate the edges first. This is necessary so that segments tile seamlessly
    //note the order in which the edges are calculated and the direction.
    //opposite edges are calculated the same way (eg left->right) so that the top
    //of one tile matches the bottom of another, likewise with sides.
    
    //temporary array used to hold each edge
    float edge[line];
    
    //calc top edge and copy into m_points
    fill1d(size,falloff,roughness,p1,p2,edge);
    for (int i=0;i<=size;i++) {
        m_points[0*line + i] = edge[i];
	checkMaxMin(edge[i]);
    }

    //calc left edge and copy into m_points
    fill1d(size,falloff,roughness,p1,p4,edge);
    for (int i=0;i<=size;i++) {
        m_points[i*line + 0] = edge[i];
	checkMaxMin(edge[i]);
    }
   
    //calc right edge and copy into m_points
    fill1d(size,falloff,roughness,p2,p3,edge);
    for (int i=0;i<=size;i++) {
        m_points[i*line + size] = edge[i];
	checkMaxMin(edge[i]);
    }

    //calc bottom edge and copy into m_points
    fill1d(size,falloff,roughness,p4,p3,edge);
    for (int i=0;i<=size;i++) {
        m_points[size*line + i] = edge[i];
	checkMaxMin(edge[i]);
    }
    
    //seed the RNG - this is the 5th and last seeding for the tile.
    //it was seeded once for each edge, now once for the tile.
    
    //total up control points
    float tot=p1+p2+p3+p4;
    //FIXME work out decent seed
    srand((unsigned int)(tot*10.22321));

    float f = falloff;
    int depth=0;
    
    //center of m_points is done separately
    int stride = size/2;
    m_points[stride*line + stride] = qRMD( m_points[0 * line + size/2],
                                        m_points[size/2*line + 0],
                                        m_points[size/2*line + size],
                                        m_points[size*line + size/2],
					roughness,
					f, depth);
		    

    checkMaxMin(m_points[stride*line + stride]);
    stride >>= 1;

    //skip across the m_points and fill in the points
    //alternate cross and plus shapes.
    //this is a diamond-square algorithm.
    while (stride) {
      //Cross shape - + contributes to value at X
      //+ . +
      //. X .
      //+ . +
      for (int i=stride;i<size;i+=stride*2) {
          for (int j=stride;j<size;j+=stride*2) {
              m_points[j*line + i] = qRMD(m_points[(i-stride) + (j+stride) * (line)],
                                       m_points[(i+stride) + (j-stride) * (line)],
                                       m_points[(i+stride) + (j+stride) * (line)],
                                       m_points[(i-stride) + (j-stride) * (line)],
			               roughness, f, depth);
              checkMaxMin(m_points[j*line + i]);
	  }
      }
 
      depth++;
      //Plus shape - + contributes to value at X
      //. + .
      //+ X +
      //. + .
      for (int i=stride*2;i<size;i+=stride*2) {
          for (int j=stride;j<size;j+=stride*2) {
              m_points[j*line + i] = qRMD(m_points[(i-stride) + (j) * (line)],
                                       m_points[(i+stride) + (j) * (line)],
                                       m_points[(i) + (j+stride) * (line)],
                                       m_points[(i) + (j-stride) * (line)], 
                                       roughness, f , depth);
              checkMaxMin(m_points[j*line + i]);
	  }
      }
	       
      for (int i=stride;i<size;i+=stride*2) {
          for (int j=stride*2;j<size;j+=stride*2) {
              m_points[j*line + i] = qRMD(m_points[(i-stride) + (j) * (line)],
                                       m_points[(i+stride) + (j) * (line)],
                                       m_points[(i) + (j+stride) * (line)],
                                       m_points[(i) + (j-stride) * (line)],
                                       roughness, f, depth);
              checkMaxMin(m_points[j*line + i]);
          }
      }

      stride>>=1;
      depth++;
    }
}


} // namespace Mercator
