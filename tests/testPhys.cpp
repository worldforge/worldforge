// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Damien McGinnes

#include <Mercator/Terrain.h>
#include <Mercator/Segment.h>
#include <Mercator/Intersect.h>

#include <iostream>

//test intersection using a rudimentary physics simulation
//this drops a particle onto the terrain and it bounces around a bit
int main()
{
    Mercator::Terrain terrain;
    
    terrain.setBasePoint(0, 0, 2.8);
    terrain.setBasePoint(1, 0, 7.1);
    terrain.setBasePoint(0, 1, 0.2);
    terrain.setBasePoint(1, 1, 14.7);

    Mercator::Segment * segment = terrain.getSegmentAtIndex(0, 0);

    if (segment == 0) {
        std::cerr << "Segment not created by addition of required basepoints"
                  << std::endl << std::flush;
        return 1;
    }

    segment->populate();
    
    WFMath::Point<3> pos(30.0,30.0,100.0); //starting position
    WFMath::Vector<3> vel(0.0,1.0,0.0); //starting velocity
    WFMath::Vector<3> grav(0.0,0.0,-9.8); //gravity

    WFMath::Point<3> intersection;
    WFMath::Vector<3> intnormal;
    
    float timestep = 0.1;
    float e = 0.2; //elasticity of collision
    float totalT = 20.0; //time limit 
    float par = 0.0;
    float t = timestep;

    while (totalT > timestep) {
        vel += t * grav;
        if (Mercator::Intersect(terrain, pos, vel * t, intersection, intnormal, par)) {
            //set pos to collision time, 
            //less a small amout to keep objects apart
            pos = intersection - (vel * .01 * t); 
                                                      
            WFMath::Vector<3> impulse = intnormal * (Dot(vel, intnormal) * -2);
            std::cerr << "HIT" << std::endl;
            vel = (vel + impulse) * e; //not sure of the impulse equation, but this will do
                
            if (vel.sqrMag() < 0.01) {
                //stop if velocities are small
                std::cerr << "friction stop" << std::endl;
                break;
            }
            totalT -= par*t;
            t = (1.0-par)*t;
        }
        else {
            pos += vel*t;
            totalT -= t;
            t = timestep;
        }

        std::cerr << "timeLeft:" << totalT << " end pos" << pos << " vel" << vel << std::endl;
    }
    
   return 0;
}
