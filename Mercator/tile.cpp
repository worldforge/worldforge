#include <iostream>
#include <cmath>

inline float randHalf() {
	return (float) rand() / RAND_MAX - 0.5f;
}

inline float qRMD(float nn, float fn, float ff, float nf, float roughness, float falloff, int depth) {
    float heightDifference = std::max(std::max(nn, fn), std::max(nf, ff)) -
                             std::min(std::min(nn, fn), std::min(nf, ff));
 
    return ((nn+fn+ff+nf)/4.f) + randHalf() * roughness * heightDifference / (1.f+::pow(depth,falloff));
}

void checkMaxMin(float h, float &max, float &min) 
{ 
    if (h>max) max=h;
    if (h<min) min=h;
}

//1 dimensional midpoint displacement fractal
//size must be a power of 2
//falloff is the decay of displacement as the fractal is refined
//array is size+1 long. array[0] and array[size] are filled
//      with the control points for the fractal
void fill1d(int size, float falloff, float roughness, float l, float h, float *array) {
    assert(falloff!=-1.0);

    int stride = size/2;
    //float f = falloff*falloff;

    array[0] = l;
    array[size] = h;
 
    int depth=1;
    //seed the RNG
    srand((unsigned int)(l*h*10.232313));
    while (stride) {
        for (int i=stride;i<size;i+=stride*2) {
	    float hh = array[i-stride];
	    float lh = array[i+stride];
            float hd = fabs(hh-lh);
 
            array[i] = ((hh+lh)/2.f) + randHalf() * roughness  * hd / (1+::pow(depth,falloff));//f);
	}
        stride >>= 1;
	depth+=2;
	//f*=falloff*falloff;
    }
}

//2 dimensional midpoint displacement fractal for a tile where
//edges are already filled by 1d fractals.
//size must be a power of 2
//array is size+1  * size+1 with the corners the control points.
void tile(int size, float falloff, float roughness, float p1, float p2, float p3, float p4, float *array, float &min, float &max) {
    assert(falloff!=-1.0);
    
    int line = size+1;
    //zero array. not really necessary, but nice.	
    for (int j=0;j<size*size;j++) array[j]=0;

    float edge[line];
    
    //top edge
    fill1d(size,falloff,roughness,p1,p2,edge);
    for (int i=0;i<=size;i++) {
        array[0*line + i] = edge[i];
	checkMaxMin(edge[i], max, min);
    }

    //left edge
    fill1d(size,falloff,roughness,p1,p4,edge);
    for (int i=0;i<=size;i++) {
        array[i*line + 0] = edge[i];
	checkMaxMin(edge[i], max, min);
    }
   
    //right edge
    fill1d(size,falloff,roughness,p2,p3,edge);
    for (int i=0;i<=size;i++) {
        array[i*line + size] = edge[i];
	checkMaxMin(edge[i], max, min);
    }

    //bottom edge
    fill1d(size,falloff,roughness,p4,p3,edge);
    for (int i=0;i<=size;i++) {
        array[size*line + i] = edge[i];
	checkMaxMin(edge[i], max, min);
    }
    
    //fill in the centre

    int stride = size/2;

    //total up control points
    float tot=p1+p2+p3+p4;
    //FIXME work out decent seed
    srand((unsigned int)(tot*10.22321));

    float f = falloff;
    int depth=0;
    
    //center of array is done separately
    array[stride*line + stride] = qRMD( array[0 * line + size/2],
                                        array[size/2*line + 0],
                                        array[size/2*line + size],
                                        array[size*line + size/2],
					roughness,
					f, depth);
		    

    checkMaxMin(array[stride*line + stride], max, min);
    stride >>= 1;

    //skip across the array and fill in the points
    //alternate cross and plus shapes.
    //this is a diamond-square algorithm.
    while (stride) {
      //Cross shape - + contributes to value at X
      //+ . +
      //. X .
      //+ . +
      for (int i=stride;i<size;i+=stride*2) {
          for (int j=stride;j<size;j+=stride*2) {
              array[j*line + i] = qRMD(array[(i-stride) + (j+stride) * (line)],
                                       array[(i+stride) + (j-stride) * (line)],
                                       array[(i+stride) + (j+stride) * (line)],
                                       array[(i-stride) + (j-stride) * (line)],
			               roughness, f, depth);
              checkMaxMin(array[j*line + i], max, min);
	  }
      }

      depth++;
      //f *= falloff;
      //Plus shape - + contributes to value at X
      //. + .
      //+ X +
      //. + .
      for (int i=stride*2;i<size;i+=stride*2) {
          for (int j=stride;j<size;j+=stride*2) {
              array[j*line + i] = qRMD(array[(i-stride) + (j) * (line)],
                                       array[(i+stride) + (j) * (line)],
                                       array[(i) + (j+stride) * (line)],
                                       array[(i) + (j-stride) * (line)], 
                                       roughness, f , depth);
              checkMaxMin(array[j*line + i], max, min);
	  }
      }
	       
      for (int i=stride;i<size;i+=stride*2) {
          for (int j=stride*2;j<size;j+=stride*2) {
              array[j*line + i] = qRMD(array[(i-stride) + (j) * (line)],
                                       array[(i+stride) + (j) * (line)],
                                       array[(i) + (j+stride) * (line)],
                                       array[(i) + (j-stride) * (line)],
                                       roughness, f, depth);
              checkMaxMin(array[j*line + i], max, min);
          }
      }

      stride>>=1;
      //f *= falloff;
      depth++;
    }
} 
#if 0
//simple demo
//read params from stdin and output a ppm to stdout
int main() {
    int size;
    int num;
    int num2;
    float RO,FO,p1,p2,p3,p4;

    //read in params
    std::cin >> num;
    std::cin >> num2;
    std::cin >> size;
    std::cin >> RO;
    std::cin >> FO;

    //header for ppm file
    std::cout << "P6 " << (size-1)*num + 1  << " " << (size-1)*num2 + 1 << " " << 255 << std::endl;

    //read in base points
    float base[num+1][num2+1];
    for (int k=0;k<=num2;k++) {
        for (int i=0;i<=num;i++) {
            std::cin >> base[i][k];
        }
    }

    //allocate arrays
    float* array[num];
    for (int i=0;i<num;i++) {
        array[i] = new float[size*size];
    }

    for (int k=0;k<num2;k++) {
        for (int n=0;n<num;n++) {
            //generate tile
            p1=base[n][k];
            p2=base[n][k+1];
            p3=base[n+1][k+1];
            p4=base[n+1][k];
            tile(size,FO,RO,p1,p2,p3,p4,array[n]);
        }
  
        //dump points out to ppm file
        //need to ensure dont output bottom or right for tiles that
        //are not on the bottom or right edge (due to repeated row/column)
        //calc for last row
        int lim;
        if (k==num-1) lim=size;
        else lim=size-1;
  
        //dump arrays
        for (int i=0;i<lim;i++) {
            for (int nn=0;nn<num;nn++) {
                for (int j=0;j<size-1;j++) {
                    std::cout << (unsigned char)((array[nn])[i + j*size]);
                    std::cout << (unsigned char)((array[nn])[i + j*size]);
                    std::cout << (unsigned char)((array[nn])[i + j*size]);
                }
            }
            //dump rightmost point on right edge
            std::cout << (unsigned char)((array[num-1])[i + (size-1)*size]);
            std::cout << (unsigned char)((array[num-1])[i + (size-1)*size]);
            std::cout << (unsigned char)((array[num-1])[i + (size-1)*size]);
        }

    }

    //clean up
    for (int i=0;i<num;i++) {
        delete[] array[i];
    }
}
#endif


   
 
