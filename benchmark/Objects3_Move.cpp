#include <iostream>
#include <cassert>
#include "Atlas/Message/Object.h"
#include "Atlas/Objects/Root.h"
#include "Atlas/Objects/Operation.h"
#include "Atlas/Objects/Entity.h"
#include "Atlas/Objects/loadDefaults.h"

#include "timer.h"

using namespace Atlas;
using namespace Atlas::Objects;
using namespace Atlas::Message;
using namespace std;

#define DEBUG 0
#if DEBUG
#define MAX_ITER 10.0
#else
#define MAX_ITER 10000000.0
#endif

#define USE_STRING 1

//Plain creating of sight operation: iterations=100000 CPU Time=4.42 iter/s=22624.4
//Plain creating of sight operation: iterations=1e+07 CPU Time=1.9 iter/s=5.26316e+06

class NPC
{
public:
  NPC() : id("123") {x=y=z = vx=vy=vz = 0.0;}
  Operation::Sight move(Operation::Move &op);
  string getId() {return id;}
private:
  string id;
  double x,y,z;
  double vx,vy,vz;
};

Operation::Sight NPC::move(Operation::Move &op)
{
    const vector<double>& new_vel = 
        ((Entity::GameEntity&)op->getArgs()[0])->
        getVelocity();
    vx = new_vel[0];
    vy = new_vel[1];
    vz = new_vel[2];
    
    x += vx;
    y += vy;
    z += vz;
    
    //human:
    Entity::GameEntity human;
    //Entity::GameEntity human = Entity::GameEntity::Instantiate();
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
    Operation::Move move;
    move->setArgs1((Root&)human);

    //sight:
    Operation::Sight sight;
    sight->setFrom(getId());
    sight->setArgs1((Root&)move);
    
    return sight;
}

int main(int argc, char** argv)
{
    try {
        loadDefaults("../../../protocols/atlas/spec/atlas.xml");
    } catch(DefaultLoadingException e) {
        cout << "DefaultLoadingException: "
             << e.msg << endl;
        return 1;
    }
    double i;
    TIME_ON;
    for(i=0; i<MAX_ITER; i+=1.0) {
        //human:
        Entity::GameEntity human;
        Object::ListType pos;
        human->modifyPos()[0] = i;
        human->modifyPos()[1] = i-1.0;
        human->modifyPos()[2] = i+1.0;
        
        human->modifyVelocity()[0] = i;
        human->modifyVelocity()[1] = i-1.0;
        human->modifyVelocity()[2] = i+1.0;
//        Object::ListType foo = human->getVelocity();
//        cout<<foo.size()<<":"<<foo.front().asFloat()<<","<<foo.back().asFloat()<<endl;

        //move:
        Operation::Move move;
        move->setArgs1((Root&)human);
//        Object::MapType ent = move.getArgs().front().asMap();
//        cout<<"vel0:"<<ent["velocity"].asList().front().asFloat()<<endl;

        //sight:
        Operation::Sight sight;
#if USE_STRING
        sight->setFrom("123");
#endif
        sight->setArgs1((Root&)move);
//        Object::MapType ent = sight.getArgs().front().asMap()
//          ["args"].asList().front().asMap();
//        cout<<"vel0:"<<ent["velocity"].asList().front().asFloat()<<endl;
    }
    TIME_OFF("Plain creating of sight operation");
    NPC npc1;
    double x,y,z;
    TIME_ON;
    for(i=0; i<MAX_ITER; i+=1.0) {
        //human:
        Entity::GameEntity human;
#if USE_STRING
        human->setId(npc1.getId());
#endif
        human->modifyVelocity()[0] = i;
        human->modifyVelocity()[1] = i-1.0;
        human->modifyVelocity()[2] = i+1.0;
        
        //move:
        Operation::Move move;
        move->setArgs1((Root&)human);

        Operation::Sight res_sight = npc1.move(move);
        const vector<double>& new_pos = 
            ((Entity::GameEntity&)
               ((Operation::Move&)res_sight->getArgs()[0])
             ->getArgs()[0])->
            getVelocity();
        x = new_pos[0];
        y = new_pos[1];
        z = new_pos[2];
    }
    TIME_OFF("NPC movements");
    cout<<"Resulting position: ("<<x<<","<<y<<","<<z<<")"<<endl;
    return 0;
}
