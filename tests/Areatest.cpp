#include <Mercator/Area.h>
#include <Mercator/Terrain.h>
#include <Mercator/Segment.h>
#include <Mercator/Surface.h>
#include <Mercator/AreaShader.h>
#include <Mercator/FillShader.h>

#include <cstdlib>
#include <cassert>
#include <iostream>
#include <fstream>

typedef WFMath::Point<2> Point2;

void writePGMForSurface(const std::string& fileName, int sz, Mercator::Surface* s)
{
    assert(s);
    
    std::ofstream file(fileName.c_str());
    file << "P5" << std::endl;
    file << sz << ' ' << sz << " 255" << std::endl;
    
    // now just blast out the binary
    file.write((const char*) s->getData(), sz * sz);
    file.close();
}

void testAreaShader()
{
    Mercator::Area* a1 = new Mercator::Area(1, false);
    
    WFMath::Polygon<2> p;
    p.addCorner(p.numCorners(), Point2(3, 4));
    p.addCorner(p.numCorners(), Point2(10, 10));
    p.addCorner(p.numCorners(), Point2(14, 6));
    p.addCorner(p.numCorners(), Point2(18, 4));
    p.addCorner(p.numCorners(), Point2(17, 19));
    p.addCorner(p.numCorners(), Point2(6, 20));
    p.addCorner(p.numCorners(), Point2(-1, 18));
    p.addCorner(p.numCorners(), Point2(-8, 11));
    
    a1->setShape(p);
    
    Mercator::Area* a2 = new Mercator::Area(1, false);
    
    WFMath::Polygon<2> p2;
    p2.addCorner(p2.numCorners(), Point2(25, 18));
    p2.addCorner(p2.numCorners(), Point2(72, 22));
    p2.addCorner(p2.numCorners(), Point2(60, 30));
    p2.addCorner(p2.numCorners(), Point2(27, 28));
    p2.addCorner(p2.numCorners(), Point2(25, 45));
    p2.addCorner(p2.numCorners(), Point2(3, 41));
    p2.addCorner(p2.numCorners(), Point2(-2, 20));
    a2->setShape(p2);
    
    Mercator::Terrain terrain(Mercator::Terrain::SHADED, 16);
    
    Mercator::Shader * base_shader =
          new Mercator::FillShader(Mercator::Shader::Parameters());
    terrain.addShader(base_shader, 0);

    Mercator::AreaShader* ashade = new Mercator::AreaShader(1);
    terrain.addShader(ashade, 1);
    
    terrain.setBasePoint(0, 0, -1);
    terrain.setBasePoint(0, 1, 8);
    terrain.setBasePoint(1, 0, 2);
    terrain.setBasePoint(1, 1, 11);
    terrain.setBasePoint(2, 0, 2);
    terrain.setBasePoint(2, 1, 11);
    
    terrain.addArea(a1);
   // terrain.addArea(a2);
    
    Mercator::Segment* seg = terrain.getSegment(0,0);
    assert(a1->checkIntersects(*seg));
    
    seg->populateSurfaces();
    writePGMForSurface("test1.pgm", seg->getSize(), seg->getSurfaces()[1]);
    
    
    seg = terrain.getSegment(1,0);    
    seg->populateSurfaces();
    writePGMForSurface("test2.pgm", seg->getSize(), seg->getSurfaces()[1]);
}

static const unsigned int seg_size = 8;
    
void testAddToSegment()
{
    Mercator::Area* a1 = new Mercator::Area(1, false);
    
    WFMath::Polygon<2> p;
    p.addCorner(p.numCorners(), Point2(1, 1));
    p.addCorner(p.numCorners(), Point2(6, 1));
    p.addCorner(p.numCorners(), Point2(6, 6));
    p.addCorner(p.numCorners(), Point2(1, 6));
    
    a1->setShape(p);
    
    Mercator::Segment * seg1 = new Mercator::Segment(0,0,seg_size);

    int success = a1->addToSegment(*seg1);
    assert(success == 0);

    Mercator::Segment * seg2 = new Mercator::Segment(1 * seg_size,0,seg_size);

    success = a1->addToSegment(*seg2);
    assert(success != 0);
}

int main(int argc, char* argv[])
{
    Mercator::Area* a1 = new Mercator::Area(1, false);
    
    WFMath::Polygon<2> p;
    p.addCorner(p.numCorners(), Point2(3, 4));
    p.addCorner(p.numCorners(), Point2(10, 10));
    p.addCorner(p.numCorners(), Point2(-1, 18));
    p.addCorner(p.numCorners(), Point2(-8, 11));
    
    a1->setShape(p);
    
    Mercator::Terrain terrain(Mercator::Terrain::SHADED, seg_size);

    Mercator::AreaShader* ashade = new Mercator::AreaShader(1);
    terrain.addShader(ashade, 0);
    
    terrain.setBasePoint(-2, -1, 5);
    terrain.setBasePoint(-2, 0, 2);
    terrain.setBasePoint(-2, 1, 19);
    
    terrain.setBasePoint(-1, -1, 4);
    terrain.setBasePoint(-1, 0, 6);
    terrain.setBasePoint(-1, 1, 10);
    
    terrain.setBasePoint(0, -1, 2);
    terrain.setBasePoint(0, 0, -1);
    terrain.setBasePoint(0, 1, 8);
    terrain.setBasePoint(0, 2, 11);
    
    terrain.setBasePoint(1, -1, 7);
    terrain.setBasePoint(1, 0, 2);
    terrain.setBasePoint(1, 1, 11);
    terrain.setBasePoint(1, 2, 9);
    
    terrain.setBasePoint(2, -1, 3);
    terrain.setBasePoint(2, 0, 8);
    terrain.setBasePoint(2, 1, 2);

    terrain.setBasePoint(3, -1, 6);
    terrain.setBasePoint(3, 0, 7);
    terrain.setBasePoint(3, 1, 9);
    
    terrain.addArea(a1);
    
    Mercator::Segment* seg = terrain.getSegment(0,0);
    assert(seg->getAreas().size() == 1);
    assert(seg->getAreas().count(1) == 1);
    assert(a1->checkIntersects(*seg));
    
    seg = terrain.getSegment(1,0);
    assert(seg->getAreas().size() == 1);
    assert(seg->getAreas().count(1) == 1);
    assert(a1->checkIntersects(*seg));

    WFMath::Polygon<2> clipped = a1->clipToSegment(*seg);
    assert(clipped.isValid());
    
    seg = terrain.getSegment(-1,0);
    assert(seg->getAreas().size() == 1);
    assert(seg->getAreas().count(1) == 1);
    assert(a1->checkIntersects(*seg));
    
    clipped = a1->clipToSegment(*seg);
    assert(clipped.isValid());
    
    seg = terrain.getSegment(0,1);
    assert(seg->getAreas().size() == 1);
    assert(seg->getAreas().count(1) == 1);
    assert(a1->checkIntersects(*seg));
    
    clipped = a1->clipToSegment(*seg);
    assert(clipped.isValid());

    seg = terrain.getSegment(2,0);
    assert(seg->getAreas().size() == 0);
    assert(seg->getAreas().count(1) == 0);
    assert(a1->checkIntersects(*seg) == false);

    p.clear();
    p.addCorner(p.numCorners(), Point2(3 + seg_size, 4));
    p.addCorner(p.numCorners(), Point2(10 + seg_size, 10));
    p.addCorner(p.numCorners(), Point2(-1 + seg_size, 18));
    p.addCorner(p.numCorners(), Point2(-8 + seg_size, 11));
    
    a1->setShape(p);

    terrain.updateArea(a1);

    seg = terrain.getSegment(0,0);
    assert(seg->getAreas().size() == 1);
    assert(seg->getAreas().count(1) == 1);
    assert(a1->checkIntersects(*seg));
    
    seg = terrain.getSegment(1,0);
    assert(seg->getAreas().size() == 1);
    assert(seg->getAreas().count(1) == 1);
    assert(a1->checkIntersects(*seg));

    clipped = a1->clipToSegment(*seg);
    assert(clipped.isValid());
    
    seg = terrain.getSegment(-1,0);
    assert(seg->getAreas().size() == 0);
    assert(seg->getAreas().count(1) == 0);
    assert(a1->checkIntersects(*seg) == false);
    
    seg = terrain.getSegment(0,1);
    assert(seg->getAreas().size() == 1);
    assert(seg->getAreas().count(1) == 1);
    assert(a1->checkIntersects(*seg));
    
    clipped = a1->clipToSegment(*seg);
    assert(clipped.isValid());

    seg = terrain.getSegment(2,0);
    assert(seg->getAreas().size() == 1);
    assert(seg->getAreas().count(1) == 1);
    assert(a1->checkIntersects(*seg));

    clipped = a1->clipToSegment(*seg);
    assert(clipped.isValid());

    terrain.removeArea(a1);

    seg = terrain.getSegment(0,0);
    assert(seg->getAreas().size() == 0);
    assert(seg->getAreas().count(1) == 0);

    seg = terrain.getSegment(1,0);
    assert(seg->getAreas().size() == 0);
    assert(seg->getAreas().count(1) == 0);

    seg = terrain.getSegment(-1,0);
    assert(seg->getAreas().size() == 0);
    assert(seg->getAreas().count(1) == 0);

    seg = terrain.getSegment(0,1);
    assert(seg->getAreas().size() == 0);
    assert(seg->getAreas().count(1) == 0);

    testAreaShader();

    testAddToSegment();
    
    return EXIT_SUCCESS;
}
