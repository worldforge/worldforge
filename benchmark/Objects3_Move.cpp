#include <iostream>
#include <cassert>
#include "Message/Object.h"
#include "Objects/Root.h"
#include "Objects/Operation.h"
#include "Objects/Entity.h"
#include "Objects/loadDefaults.h"

#include "timer.h"

using namespace Atlas;
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
  Sight move(Move &op);
  string getId() {return id;}
private:
  string id;
  double x,y,z;
  double vx,vy,vz;
};

Sight NPC::move(Move &op)
{
    const vector<double>& new_vel = 
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
    move->setArgs1((Root&)human);

    //sight:
    Sight sight;
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
        GameEntity human;
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
        Move move;
        move->setArgs1((Root&)human);
//        Object::MapType ent = move.getArgs().front().asMap();
//        cout<<"vel0:"<<ent["velocity"].asList().front().asFloat()<<endl;

        //sight:
        Sight sight;
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
        GameEntity human;
#if USE_STRING
        human->setId(npc1.getId());
#endif
        human->modifyVelocity()[0] = i;
        human->modifyVelocity()[1] = i-1.0;
        human->modifyVelocity()[2] = i+1.0;
        
        //move:
        Move move;
        move->setArgs1((Root&)human);

        Sight res_sight = npc1.move(move);
        const vector<double>& new_pos = 
            ((GameEntity&)
               ((Move&)res_sight->getArgs()[0])
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
