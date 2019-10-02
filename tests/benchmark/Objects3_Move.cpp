#include "timer.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>
#include <Atlas/Objects/loadDefaults.h>

#include <iostream>
#include <cassert>

using Atlas::Objects::Root;
using Atlas::Objects::Operation::Move;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Entity::GameEntity;

//#define DEBUG 0
#define MAX_ITER 10000000.0

#define USE_STRING 1

//Plain creating of sight operation: iterations=100000 CPU Time=4.42 iter/s=22624.4
//Plain creating of sight operation: iterations=1e+07 CPU Time=1.9 iter/s=5.26316e+06

class NPC
{
public:
  NPC() : id("123") {x=y=z = vx=vy=vz = 0.0;}
  Sight move(Move &op);
  std::string getId() {return id;}
private:
  std::string id;
  double x,y,z;
  double vx,vy,vz;
};

Sight NPC::move(Move &op)
{
    const std::vector<double>& new_vel = 
        ((GameEntity&)op->getArgs()[0])->
        getVelocity();
    vx = new_vel[0];
    vy = new_vel[1];
    vz = new_vel[2];
    
    x += vx;
    y += vy;
    z += vz;
    
    //human:
    GameEntity human;
    //GameEntity human = GameEntity::Instantiate();
#if USE_STRING
    human->setId(getId());
#endif
    human->modifyPos()[0] = x;
    human->modifyPos()[1] = y;
    human->modifyPos()[2] = z;
    human->modifyVelocity()[0] = vx;
    human->modifyVelocity()[1] = vy;
    human->modifyVelocity()[2] = vz;
    
    //move:
    Move move;
    move->setArgs1(human);

    //sight:
    Sight sight;
    sight->setFrom(getId());
    sight->setArgs1(move);
    
    return sight;
}

int main(int argc, char** argv)
{
	Atlas::Objects::Factories factories;
    try {
        Atlas::Objects::loadDefaults("../../data/protocols/atlas/spec/atlas.xml", factories);
    } catch(const Atlas::Objects::DefaultLoadingException& e) {
        std::cout << "DefaultLoadingException: "
             << e.getDescription() << std::endl;
    }
    double i;
    TIME_ON
    for(i=0; i<MAX_ITER; i+=1.0) {
        //human:
        GameEntity human;
        Atlas::Message::ListType pos;
        human->modifyPos()[0] = i;
        human->modifyPos()[1] = i-1.0;
        human->modifyPos()[2] = i+1.0;
        
        human->modifyVelocity()[0] = i;
        human->modifyVelocity()[1] = i-1.0;
        human->modifyVelocity()[2] = i+1.0;
//        Object::ListType foo = human->getVelocity();
//        std::cout<<foo.size()<<":"<<foo.front().asFloat()<<","<<foo.back().asFloat()<<std::endl;

        //move:
        Move move;
        move->setArgs1(human);
//        Object::MapType ent = move.getArgs().front().asMap();
//        std::cout<<"vel0:"<<ent["velocity"].asList().front().asFloat()<<std::endl;

        //sight:
        Sight sight;
#if USE_STRING
        sight->setFrom("123");
#endif
        sight->setArgs1(move);
//        Object::MapType ent = sight.getArgs().front().asMap()
//          ["args"].asList().front().asMap();
//        std::cout<<"vel0:"<<ent["velocity"].asList().front().asFloat()<<std::endl;
    }
    TIME_OFF("Plain creating of sight operation");
    NPC npc1;
    double x = 0., y = 0., z = 0.;
    TIME_ON;
    for(i=0; i<MAX_ITER; i+=1.0) {
        //human:
        GameEntity human;
#if USE_STRING
        human->setId(npc1.getId());
#endif
        human->modifyVelocity()[0] = i;
        human->modifyVelocity()[1] = i-1.0;
        human->modifyVelocity()[2] = i+1.0;
        
        //move:
        Move move;
        move->setArgs1(human);

        Sight res_sight = npc1.move(move);
        const std::vector<double>& new_pos = 
            ((GameEntity&)
               ((Move&)res_sight->getArgs()[0])
             ->getArgs()[0])->
            getVelocity();
        x = new_pos[0];
        y = new_pos[1];
        z = new_pos[2];
    }
    TIME_OFF("NPC movements");
    std::cout<<"Resulting position: ("<<x<<","<<y<<","<<z<<")"<<std::endl;
    return 0;
}
