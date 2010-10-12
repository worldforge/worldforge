// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2010 Alistair Riddoch

#include <Mercator/Terrain.h>

#include <Mercator/Area.h>
#include <Mercator/Segment.h>
#include <Mercator/Shader.h>
#include <Mercator/Surface.h>

#include <wfmath/point.h>

// This is a relatively specific tests which ensures Terrain::addArea
// behaves as defined with respect to invalidating surfaces, to verify
// a refactoring. Previous cases where std::map::count() was used followed
// by [] are inefficient, and I wanted to verify consistent bahviour before
// checking to use std::map::find()

class TestShader : public Mercator::Shader {
  public:
    virtual bool checkIntersect(const Mercator::Segment &) const
    {
        return true;
    }

    /// \brief Populate a Surface with data.
    virtual void shade(Mercator::Surface &) const { }
};

int main()
{
    Mercator::Terrain t;

    // Add a null test shader to the terrain
    Mercator::Shader * shader = new TestShader;
    t.addShader(shader, 0);

    // Create a test area with a shape which intersects
    // the Segment at 0,0
    Mercator::Area* a1 = new Mercator::Area(1, false);
    
    WFMath::Polygon<2> p;
    p.addCorner(p.numCorners(), WFMath::Point<2>(3, 4));
    p.addCorner(p.numCorners(), WFMath::Point<2>(10, 10));
    p.addCorner(p.numCorners(), WFMath::Point<2>(14, 6));
    p.addCorner(p.numCorners(), WFMath::Point<2>(18, 4));
    p.addCorner(p.numCorners(), WFMath::Point<2>(17, 19));
    p.addCorner(p.numCorners(), WFMath::Point<2>(6, 20));
    p.addCorner(p.numCorners(), WFMath::Point<2>(-1, 18));
    p.addCorner(p.numCorners(), WFMath::Point<2>(-8, 11));
    
    a1->setShape(p);

    // Add enough base points to force the creation of the Segment at 0,0
    t.setBasePoint(0, 0, -1);
    t.setBasePoint(0, 1, 8);
    t.setBasePoint(1, 0, 2);
    t.setBasePoint(1, 1, 11);

    // Get the Segment at 0,0
    Mercator::Segment * seg = t.getSegment(0,0);

    assert(seg != 0);

    // Get the surfaces, and add one corresponding to the shader we added.
    // We need to do this as the functions that would normally make it happen
    // have been stubbed out.
    Mercator::Segment::Surfacestore & sss = seg->getSurfaces();

    Mercator::Surface * sfce = new Mercator::Surface(*seg, *shader);

    // Force allocation of the surface buffer so we can check later that it
    // gets destroyed when the area is added to the terrain.
    sfce->allocate();
    assert(sfce->isValid());

    // Add the surface to the store for this segment
    sss[0] = sfce;

    // Add the area which should cause relevant surface date to be invalidated
    t.addArea(a1);

    // We assert this to ensure that the buffer has been de-allocated
    // by a call to Surface::invalidate caused by adding the Area.
    assert(!sfce->isValid());

    // force the surface to re-allocate
    sfce->allocate();
    assert(sfce->isValid());

    // Modify the areas shape
    p.addCorner(p.numCorners(), WFMath::Point<2>(-9, 12));
    a1->setShape(p);

    // and cause an area update
    t.updateArea(a1);

    // Check the surface has been invalidated again
    assert(!sfce->isValid());

    // force the surface to re-allocate
    sfce->allocate();
    assert(sfce->isValid());

    t.removeArea(a1);

    // Check the surface has been invalidated again
    assert(!sfce->isValid());
}

// stubs

#include <Mercator/Area.h>
#include <Mercator/Buffer_impl.h>
#include <Mercator/Shader.h>

namespace Mercator {

Segment::Segment(int x, int y, unsigned int r) : m_res(r), m_size(r + 1),
                                                 m_xRef(x), m_yRef(y)
{
}

Segment::~Segment()
{
}

void Segment::populateSurfaces()
{
}

int Segment::addArea(const Area *)
{
    return 0;
}

int Segment::updateArea(const Area *)
{
    invalidateSurfaces();
    return 0;
}

int Segment::removeArea(const Area *)
{
    invalidateSurfaces();
    return 0;
}

void Segment::addMod(const TerrainMod *)
{
}

void Segment::removeMod(TerrainMod *)
{
}

void Segment::invalidate(bool)
{
}

bool Segment::clipToSegment(const WFMath::AxisBox<2> &,
                            int &,
                            int &,
                            int &,
                            int &) const
{
    return true;
}

void Segment::getHeightAndNormal(float, float,
                                 float &,
                                 WFMath::Vector<3> &) const
{
}

WFMath::AxisBox<2> Segment::getRect() const
{
    WFMath::Point<2> lp(m_xRef, m_yRef), 
        hp(lp.x() + m_res, lp.y() + m_res);
    return WFMath::AxisBox<2>(lp, hp);
}

void Segment::invalidateSurfaces()
{
    Segment::Surfacestore::const_iterator I = m_surfaces.begin();
    Segment::Surfacestore::const_iterator Iend = m_surfaces.end();
    for(; I != Iend; ++I) {
        I->second->invalidate();
    }
}

Shader::Shader(bool color, bool alpha) : m_color(color), m_alpha(alpha)
{
}

Shader::~Shader()
{
}

Surface * Shader::newSurface(const Segment &) const
{
    return 0;
}

Surface::Surface(const Segment & seg, const Shader & sh, bool color, bool alpha)
  : Buffer<ColorT>(seg, (color ? 3 : 0) + (alpha ? 1 : 0)), m_shader(sh)
{
}

Surface::~Surface()
{
}

const float BasePoint::HEIGHT = 8.0;
const float BasePoint::ROUGHNESS = 1.25;
const float BasePoint::FALLOFF = 0.25;

}
