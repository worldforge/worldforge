#include <Mercator/Intersect.h>
#include <Mercator/Segment.h>

namespace Mercator {
//floor and ceil functions that return d-1 and d+1
//respectively if d is integral
float gridceil(float d) 
{
    float c = ceil(d);
    return (c==d) ? c+1.0f : c;
}

float gridfloor(float d) 
{
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

bool Intersect(const Terrain &t, const WFMath::Point<3> &pt) 
{
    return HOT(t, pt) <= 0.0;
}

float HOT(const Terrain &t, const WFMath::Point<3> &pt) 
{
    WFMath::Vector<3> normal; 
    float terrHeight;
    t.getHeightAndNormal(pt[0], pt[1], terrHeight, normal);

    return (pt[2] - terrHeight);
}

//Intersection of segment with terrain
bool Intersect(const Terrain &t, const WFMath::Point<3> &sPt, const WFMath::Vector<3>& dir,
                float &par)
{
    float paraX, paraY, pX, pY, crossX, crossY;
    float h1,h2,h3,h4,height;

    WFMath::Point<3> last(sPt), next(sPt);
    
    if (dir[0] != 0.0f) {
        paraX = 1.0f/dir[0];
        crossX = (dir[0] > 0.0f) ? gridceil(last[0]) : gridfloor(last[0]);
        pX = (crossX - last[0]) * paraX;
        pX = std::min(pX, 1.0f);
    }
    else
        pX = 1.0f;

    if (dir[1] != 0.0f) {
        paraY = 1.0f/dir[1];
        crossY = (dir[1] > 0.0f) ? gridceil(last[1]) : gridfloor(last[1]);
        pY = (crossY - sPt[1]) * paraY;
        pY = std::min(pY, 1.0f);
    }
    else
        pY = 1.0f;

    paraX = std::abs(paraX);
    paraY = std::abs(paraY);

    while (1) {
        last = next;
        if (pX < pY) { // cross x grid line first
            next = sPt + (pX * dir);
            pX += paraX;
            crossX = (dir[0] > 0.0f) ? gridceil(last[0]) : gridfloor(last[0]);
        }
        else { //cross y grid line first
            next = sPt + (pY * dir);
            if (pX==pY) pX += paraX; //unusual case where ray crosses corner
            pY += paraY;
            crossY = (dir[1] > 0.0f) ? gridceil(last[1]) : gridfloor(last[1]);
        }

        //FIXME these gets could be optimized a bit
        h1 = t.get(crossX, crossY);
        h2 = t.get(crossX, crossY+1);
        h3 = t.get(crossX+1, crossY+1);
        h4 = t.get(crossX, crossY+1);
        height = std::max(std::max(h1,h2), std::max(h3,h4)); 
        if ( (last[2] < height) || (next[2] < height) ) {
            // intersect with this tile
        std::cerr << " INTERSECT" << std::endl;
        //FIXME insert detailed intersection tests here
            return false;
        }

        if ((pX >= 1.0f) && (pY >= 1.0f)) break;
    }

    return true;
}


} // namespace Mercator

