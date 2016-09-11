// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include <Mercator/Terrain.h>
#include <Mercator/Segment.h>
#include <Mercator/FillShader.h>

#include <wfmath/const.h>

#include <iostream>
#include <cstdlib>

int main()
{
    {
        Mercator::Terrain::Pointcolumn mtpc;
        Mercator::Terrain::Segmentcolumn mtsc;
        Mercator::Terrain::Pointstore mtps;
        Mercator::Terrain::Segmentstore mtss;
    }

    {
        Mercator::Terrain fineTerrain(Mercator::Terrain::DEFAULT, 8);

        unsigned int res = fineTerrain.getResolution();

        if (res != 8) {
            std::cerr << "Terrain with res specified does not have correct res."
                      << std::endl << std::flush;
            std::cerr << "Resolution " << 8 << " was specified, but "
                      << res << " was returned."
                      << std::endl << std::flush;
            return 1;
        }
    }

    Mercator::Terrain terrain(Mercator::Terrain::SHADED, 64);

    unsigned int res = terrain.getResolution();

    if (res != Mercator::defaultResolution) {
        std::cerr << "Terrain with no res specified does not have default"
                  << std::endl << std::flush;
        std::cerr << "Resolution " << Mercator::defaultResolution
                  << " is default, but " << res << " was returned."
                  << std::endl << std::flush;
        return 1;
    }

    for (int i = 0; i < 100; ++i) {
        float testHeight = terrain.get((float)rand(), (float)rand());
        if (testHeight != Mercator::Terrain::defaultLevel) {
            std::cerr << "Randomly selected point in empty terrain object was not default height"
                      << std::endl << std::flush;
            std::cerr << "Point had height " << testHeight
                      << " but default is " << Mercator::Terrain::defaultLevel
                      << std::endl << std::flush;
            return 1;
        }
    }

    Mercator::Segment * seg = terrain.getSegment(rand(), rand());
    if (seg != 0) {
        std::cerr << "Randomly selected segment position did not return NULL segment pointer on empty terrain"
                  << std::endl << std::flush;
        return 1;
    }

    const Mercator::Terrain::Segmentstore & tSegments = terrain.getTerrain();
    const Mercator::Terrain::Pointstore & tPoints = terrain.getPoints();
    const Mercator::Terrain::Shaderstore & tShaders = terrain.getShaders();

    if (!tSegments.empty()) {
        std::cerr << "Segment store for empty terrain is not empty"
                  << std::endl << std::flush;
        return 1;
    }

    if (!tPoints.empty()) {
        std::cerr << "Point store for empty terrain is not empty"
                  << std::endl << std::flush;
        return 1;
    }

    if (!tShaders.empty()) {
        std::cerr << "Shader store for empty terrain is not empty"
                  << std::endl << std::flush;
        return 1;
    }

    terrain.addShader(new Mercator::FillShader(), 0);

    if (tShaders.empty()) {
        std::cerr << "Shader store for terrain is empty after shader was added"
                  << std::endl << std::flush;
        return 1;
    }

    terrain.setBasePoint(0, 0, 2.8);
    terrain.setBasePoint(1, 0, -7.1);
    terrain.setBasePoint(0, 1, 0.2);
    terrain.setBasePoint(1, 1, 14.7);

    if (tPoints.empty()) {
        std::cerr << "Point store for populated terrain is empty"
                  << std::endl << std::flush;
        return 1;
    }

    if (tSegments.empty()) {
        std::cerr << "Segment store for populated terrain is empty"
                  << std::endl << std::flush;
        return 1;
    }

    Mercator::BasePoint bp;
    terrain.getBasePoint(0, 0, bp);
    if (bp.seed() != 2800) {
        std::cerr << "BasePoint seed calculation is incorrect."
                  << std::endl << std::flush;
        return 1;
    }
    terrain.getBasePoint(1, 0, bp);
    if (bp.seed() != -7100U) {
        std::cerr << "BasePoint seed calculation is incorrect."
                  << std::endl << std::flush;
        return 1;
    }
    terrain.getBasePoint(0, 1, bp);
    if (bp.seed() != 200) {
        std::cerr << "BasePoint seed calculation is incorrect."
                  << std::endl << std::flush;
        return 1;
    }
    terrain.getBasePoint(1, 1, bp);
    if (bp.seed() != 14700) {
        std::cerr << "BasePoint seed calculation is incorrect."
                  << std::endl << std::flush;
        return 1;
    }

    Mercator::Segment * segment = terrain.getSegment(0, 0);

    if (segment == 0) {
        std::cerr << "Segment not created by addition of required basepoints"
                  << std::endl << std::flush;
        return 1;
    }

    Mercator::Segment::Surfacestore & surfaces = segment->getSurfaces();
    
    if (surfaces.size() != tShaders.size()) {
        std::cerr << "Number of surfaces in the Segment does not match number of shaders on the terrain"
                  << std::endl << std::flush;
        return 1;
    }

    //Now let's check that the underlying algorithm used for calculating heights is intact.
    //We've previously determined three height values from sampling random points in the terrain.
    //Given the previous input to the terrain system these heights should always return the same value.
    //If not, something has changed regarding the algorithm used for generating the terrain.
    segment->populate();
    float height = 0;
    WFMath::Vector<3> normal;
    terrain.getHeightAndNormal(5.5f, 6.5f, height, normal);
    if (!WFMath::Equal(height, 2.00456953f)) {
        std::cerr << "Height sampling is incorrect. This is caused by the underlying algorithm being changed."
                  << std::endl << std::flush;
        return 1;
    }
    terrain.getHeightAndNormal(15.5f, 12.6f, height, normal);
    if (!WFMath::Equal(height, 2.221810579f)) {
        std::cerr << "Height sampling is incorrect. This is caused by the underlying algorithm being changed."
                  << std::endl << std::flush;
        return 1;
    }
    terrain.getHeightAndNormal(20.2f, 17.4f, height, normal);
    if (!WFMath::Equal(height, 0.9405912161f)) {
        std::cerr << "Height sampling is incorrect. This is caused by the underlying algorithm being changed."
                  << std::endl << std::flush;
        return 1;
    }

    return 0;
}
