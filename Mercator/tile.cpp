#include <iostream>
#include <cmath>

//1 dimensional midpoint displacement fractal
//size must be a power of 2
//roughness is the max displacement value
//falloff is the decay of displacement as the fractal is refined
//array is size+1 long. array[0] and array[size] are filled
//      with the control points for the fractal
void fill1d(int size, float falloff, float roughness, float *array) {
    assert(falloff!=-1.0);

    int stride=size/2;
    float f=falloff;

    //seed the RNG
    srand((unsigned int)(array[0]*array[size]));
    while (stride) {
        for (int i=stride;i<size;i+=stride*2) {
	    float disp=( (float)rand() / RAND_MAX ) - 0.5f;
	    float h=array[i-stride];
	    float l=array[i+stride];
	    float diff=h-l;
            array[i]=((h+l)/2.f) + disp * roughness * 2.f/(1.f + f);
        }
        stride>>=1;
	f*=falloff;
    }
}

//2 dimensional midpoint displacement fractal for a tile where
//edges are already filled by 1d fractals.
//size must be a power of 2
//roughness is the max displacement value
//falloff is the decay of displacement as the fractal is refined
//array is size+1  * size+1 with the corners the control points.
void fill2d(int size, float falloff, float roughness, float *array) {
    assert(falloff!=-1.0);
    
    int stride=size/2;
    int line=size+1;

    //total up control cpoints
    float tot=array[0 + size*0];
    tot+=array[size + size*0];
    tot+=array[size + size*line];
    tot+=array[0 + size*line];
    //FIXME work out decent seed
    srand((unsigned int)tot);
   
    float disp=( (float)rand() / (float)RAND_MAX )- 0.5f;

    //center of array is done separately
    array[stride*line + stride] = (tot / 4.f) + disp * roughness 
	                           * (2. / (1.f + falloff));
    stride >>= 1;
    float fall=falloff;

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
	      disp=( (float)rand() / (float)RAND_MAX )- 0.5f;
              array[j*line + i]=((array[(i-stride) + (j+stride) * (line)]+
                                  array[(i+stride) + (j-stride) * (line)]+
                                  array[(i+stride) + (j+stride) * (line)]+
                                  array[(i-stride) + (j-stride) * (line)])
			          /4.f) + disp * roughness * 2.f/(1.f+fall);
	  }
      }

      //Plus shape - + contributes to value at X
      //. + .
      //+ X +
      //. + .
      for (int i=stride*2;i<size;i+=stride*2) {
          for (int j=stride;j<size;j+=stride*2) {
	      disp=( (float)rand() / (float)RAND_MAX )- 0.5f;
              array[j*line + i]= ((array[(i-stride) + (j) * (line)]+
                                   array[(i+stride) + (j) * (line)]+
                                   array[(i) + (j+stride) * (line)]+
                                   array[(i) + (j-stride) * (line)]) / 4.f) 
                                   + disp * roughness * 2.f/(1.f+fall);
	  }
      }
	       
      for (int i=stride;i<size;i+=stride*2) {
          for (int j=stride*2;j<size;j+=stride*2) {
              disp=( (float)rand() / (float)RAND_MAX )- 0.5f;
              array[j*line + i]= ((array[(i-stride) + (j) * (line)]+
                                   array[(i+stride) + (j) * (line)]+
                                   array[(i) + (j+stride) * (line)]+
                                   array[(i) + (j-stride) * (line)]) / 4.f) 
                                   + disp * roughness * 2.f/(1.f+fall);
          }
      }

      stride>>=1;
      fall*=falloff;
    }
}

//generate the tile 
//size is number of points on each side of the tile 
//must be power of 2 + 1
//roughness is the max displacement value
//falloff is the decay of displacement as the fractal is refined
//array is of size+1 * size+1
void tile(int size, float falloff, float roughness, 
	  float p1, float p2, float p3, float p4, float *array) {
	
    //zero array. not really necessary, but nice.	
    for (int j=0;j<size*size;j++) array[j]=0;


    float edge[size];
    
    //top edge
    edge[0]=p1;
    edge[size-1]=p2;
    fill1d(size-1,falloff,roughness,edge);
    for (int i=0;i<size;i++) {
        array[0*size + i] = edge[i];
    }

    //left edge
    edge[0]=p1;
    edge[size-1]=p4;
    fill1d(size-1,falloff,roughness,edge);
    for (int i=0;i<size;i++) {
        array[i*size + 0] = edge[i];
    }
   
    //right edge
    edge[0]=p2;
    edge[size-1]=p3;
    fill1d(size-1,falloff,roughness,edge);
    for (int i=0;i<size;i++) {
        array[i*size + (size-1)] = edge[i];
    }

    //bottom edge
    edge[0]=p4;
    edge[size-1]=p3;
    fill1d(size-1,falloff,roughness,edge);
    for (int i=0;i<size;i++) {
        array[(size-1)*size + i] = edge[i];
    }
    
    //fill in the centre
    fill2d(size-1,falloff,roughness,array);
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
