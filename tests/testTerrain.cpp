// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include <Mercator/Terrain.h>

#include <iostream>

int main()
{
    {
        Mercator::Terrain::Pointcolumn mtpc;
        Mercator::Terrain::Segmentcolumn mtsc;
        Mercator::Terrain::Pointstore mtps;
        Mercator::Terrain::Segmentstore mtss;
    }

    {
        Mercator::Terrain fineTerrain(8);

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

    Mercator::Terrain terrain;

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
    }

    if (!tPoints.empty()) {
        std::cerr << "Point store for empty terrain is not empty"
                  << std::endl << std::flush;
    }

    if (!tShaders.empty()) {
        std::cerr << "Shader store for empty terrain is not empty"
                  << std::endl << std::flush;
    }

    return 0;
}
