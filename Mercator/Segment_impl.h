namespace Mercator {

//level a shape of terrain at altitude 'level'
template <typename Shape>
void Segment::levelShape(const Shape &s, float level)
{
    int lx,hx,ly,hy;
    if (clipToSegment(s.boundingBox(), lx, hx, ly, hy)) {
        for (int i=ly;i<=hy;i++) {
	    for (int j=lx;j<=hx;j++) {
		if (Contains(WFMath::Point<2>(j,i),s,true)) {
                    m_points[i * (m_res + 1) + j] = level;
		}
	    }
	}
    }
}

//raise or lower a shape of terrain by dist units
template <typename Shape>
void Segment::modifyShape(const Shape &s, float dist)
{
    int lx,hx,ly,hy;
    if (clipToSegment(s.boundingBox(), lx, hx, ly, hy)) {
        for (int i=ly;i<=hy;i++) {
	    for (int j=lx;j<=hx;j++) {
		if (Contains(WFMath::Point<2>(j,i),s,true)) {
                    m_points[i * (m_res + 1) + j] += dist;
		}
	    }
	}
    }
}

//make a patch of terrain sloped by gradX, gradY. level is altitude at the center
template <typename Shape>
void Segment::slopeShape(const Shape &s, float level, float dX, float dY)
{
    int lx,hx,ly,hy;
    if (clipToSegment(s.boundingBox(), lx, hx, ly, hy)) {
	float gradX = gradX/m_res;
	float gradY = gradY/m_res;
        for (int i=ly;i<=hy;i++) {
	    for (int j=lx;j<=hx;j++) {
		if (Contains(WFMath::Point<2>(j,i),s,true)) {
                    m_points[i * (m_res + 1) + j] = level + (centerY-i) * gradY + (centerX-j) * gradX;
		}
	    }
	}
    }
}

} //namespace Mercator
