#include <Mercator/Intersect.h>
#include <Mercator/Segment.h>

namespace Mercator {
//floor and ceil functions that return d-1 and d+1
//respectively if d is integral
float gridceil(float d) {
    float c = ceil(d);
    return (c==d) ? c+1.0f : c;
}

float gridfloor(float d) {
    float c = floor(d);
    return (c==d) ? c-1.0f : c;
}

//check intersection of an axis-aligned box with the terrain
bool Intersect(const Terrain &t, const WFMath::AxisBox<3> &bbox)
{
    float max, min=bbox.lowCorner()[2];
    int res = t.getResolution();

    //determine which segments are involved
    //usually will just be one
    int xlow  = (int) floor(bbox.lowCorner()[0] / res);
    int xhigh = (int) gridceil(bbox.highCorner()[0] / res);
    int ylow  = (int) floor(bbox.lowCorner()[1] / res);
    int yhigh = (int) gridceil(bbox.highCorner()[1] / res);

    //loop across all tiles covered by this bbox
    for (int x = xlow; x < xhigh; x++) {
        for (int y = ylow; y < yhigh; y++) {
            //check the bbox against the extent of each tile
            //as an early rejection
            Segment *thisSeg=t.getSegment(x,y);

            if (thisSeg)
                max=thisSeg->getMax();
            else
                max=Terrain::defaultLevel;

            if (max > min) {
	        //entity bbox overlaps with the extents of this tile
		//now check each tile point covered by the entity bbox
                
                //clip the points to be tested against the bbox
                int min_x = (int) floor(bbox.lowCorner()[0] - (x * res));
                if (min_x < 0) min_x = 0;

                int max_x = (int) gridceil(bbox.highCorner()[0] - (x * res));
                if (max_x > res) min_x = res;
                
                int min_y = (int) floor(bbox.lowCorner()[1] - (y * res));
                if (min_y < 0) min_y = 0;

                int max_y = (int) gridceil(bbox.highCorner()[1] - (y * res));
                if (max_y > res) min_y = res;

                //loop over each point and see if it is greater than the minimum
                //of the bbox. If all points are below, the the bbox does NOT
                //intersect. If a single point is above, then the bbox MIGHT 
                //intersect.
                for (int xpt = min_x; xpt <= max_x; xpt++) {
		    for (int ypt = min_y; ypt <= max_y; ypt++) {
			if (thisSeg) { 
        	            if (thisSeg->get(xpt,ypt) > min) return true;
			}
			else if (Terrain::defaultLevel > min) return true;
                    }
                }
            }
        }
    }
    return false;
}

} // namespace Mercator

