#include <iostream>
#include <cassert>
#include "../src/Message/Object.h"
#include "../src/Objects/Root.h"
#include "../src/Objects/Operation.h"
#include "../src/Objects/Entity.h"
#include "../src/Objects/LoadDefaults.h"

#include "timer.h"

using namespace Atlas;
using namespace Atlas::Objects;
using namespace Atlas::Message;
using namespace std;

#define DEBUG 0
#if DEBUG
#define MAX_ITER 10.0
#else
#define MAX_ITER 1000000.0
#endif

#define USE_STRING 1

//Plain creating of sight operation: iterations=100000 CPU Time=4.42 iter/s=22624.4
//Plain creating of sight operation: iterations=1e+07 CPU Time=1.9 iter/s=5.26316e+06

class NPC
{
public:
  NPC() : id("123") {x=y=z = vx=vy=vz = 0.0;}
  Operation::Sight move(Operation::Move &op);
  string GetId() {return id;}
private:
  string id;
  double x,y,z;
  double vx,vy,vz;
};

Operation::Sight NPC::move(Operation::Move &op)
{
    const vector<double>& new_vel = 
        ((Entity::GameEntity&)op->GetArgs()[0])->
        GetVelocity();
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
    human->SetId(GetId());
#endif
    human->ModifyPos()[0] = x;
    human->ModifyPos()[1] = y;
    human->ModifyPos()[2] = z;
    human->ModifyVelocity()[0] = vx;
    human->ModifyVelocity()[1] = vy;
    human->ModifyVelocity()[2] = vz;
    
    //move:
    Operation::Move move;
    move->SetArgs1((Root&)human);

    //sight:
    Operation::Sight sight;
    sight->SetFrom(GetId());
    sight->SetArgs1((Root&)move);
    
    return sight;
}

#include "../src/Codecs/XML.cpp"

int main(int argc, char** argv)
{
    try {
        LoadDefaults("../../../protocols/atlas/spec/atlas.xml");
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
        human->ModifyPos()[0] = i;
        human->ModifyPos()[1] = i-1.0;
        human->ModifyPos()[2] = i+1.0;
        
        human->ModifyVelocity()[0] = i;
        human->ModifyVelocity()[1] = i-1.0;
        human->ModifyVelocity()[2] = i+1.0;
//        Object::ListType foo = human->GetVelocity();
//        cout<<foo.size()<<":"<<foo.front().AsFloat()<<","<<foo.back().AsFloat()<<endl;

        //move:
        Operation::Move move;
        move->SetArgs1((Root&)human);
//        Object::MapType ent = move.GetArgs().front().AsMap();
//        cout<<"vel0:"<<ent["velocity"].AsList().front().AsFloat()<<endl;

        //sight:
        Operation::Sight sight;
#if USE_STRING
        sight->SetFrom("123");
#endif
        sight->SetArgs1((Root&)move);
//        Object::MapType ent = sight.GetArgs().front().AsMap()
//          ["args"].AsList().front().AsMap();
//        cout<<"vel0:"<<ent["velocity"].AsList().front().AsFloat()<<endl;
    }
    TIME_OFF("Plain creating of sight operation");
    NPC npc1;
    double x,y,z;
    TIME_ON;
    for(i=0; i<MAX_ITER; i+=1.0) {
        //human:
        Entity::GameEntity human;
#if USE_STRING
        human->SetId(npc1.GetId());
#endif
        human->ModifyVelocity()[0] = i;
        human->ModifyVelocity()[1] = i-1.0;
        human->ModifyVelocity()[2] = i+1.0;
        
        //move:
        Operation::Move move;
        move->SetArgs1((Root&)human);

        Operation::Sight res_sight = npc1.move(move);
        const vector<double>& new_pos = 
            ((Entity::GameEntity&)
               ((Operation::Move&)res_sight->GetArgs()[0])
             ->GetArgs()[0])->
            GetVelocity();
        x = new_pos[0];
        y = new_pos[1];
        z = new_pos[2];
    }
    TIME_OFF("NPC movements");
    cout<<"Resulting position: ("<<x<<","<<y<<","<<z<<")"<<endl;
    return 0;
}
