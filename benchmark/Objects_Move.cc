#include <iostream>
#include <cassert>
#include "../src/Message/Object.h"
#include "../src/Objects/Root.h"
#include "../src/Objects/Operation/Move.h"
#include "../src/Objects/Operation/Sight.h"
#include "../src/Objects/Entity/GameEntity.h"

#include "timer.h"

using namespace Atlas;
using namespace Atlas::Objects;
using namespace Atlas::Message;
using namespace std;

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
  Object::ListType::iterator new_vel_i = op.GetArgs().
    front().AsMap()["velocity"].AsList().begin();
  vx = (*new_vel_i).AsFloat();
  new_vel_i++; vy = (*new_vel_i).AsFloat();
  new_vel_i++; vz = (*new_vel_i).AsFloat();

  x += vx;
  y += vy;
  z += vz;

  //human:
  Entity::GameEntity human = Entity::GameEntity::Instantiate();
  human.SetId(GetId());
  Object::ListType pos;
  pos.push_back(x);
  pos.push_back(y);
  pos.push_back(z);
  human.SetPos(pos);
  Object::ListType vel;
  vel.push_back(vx);
  vel.push_back(vy);
  vel.push_back(vz);
  human.SetVelocity(vel);

  //move:
  Operation::Move move = Operation::Move::Instantiate();
  Object::ListType move_args;
  move_args.push_back(human.AsObject());
  move.SetArgs(move_args);

  //sight:
  Operation::Sight sight = Operation::Sight::Instantiate();
  sight.SetFrom(GetId());
  Object::ListType sight_args;
  sight_args.push_back(move.AsObject());
  sight.SetArgs(sight_args);

  return sight;
}

int main(int argc, char** argv)
{
  double i;
  TIME_ON;
  for(i=0; i<10000.0; i+=1.0) {
    //human:
    Entity::GameEntity human = Entity::GameEntity::Instantiate();
    Object::ListType pos;
    pos.push_back(i);
    pos.push_back(i-1.0);
    pos.push_back(i+1.0);
    human.SetPos(pos);
#if 1 //12.72user 0.00system 0:14.97elapsed 84%CPU
    Object::ListType vel;
    vel.push_back(i);
    vel.push_back(i-1.0);
    vel.push_back(i+1.0);
    human.SetVelocity(vel);
#else //13.95user 0.00system 0:16.83elapsed 82%CPU
    Object::ListType::iterator veli = human.GetVelocity().begin();
    *veli = i; veli++;
    *veli = i+1.0; veli++;
    *veli = i-1.0;
    
//    human.GetVelocity().push_back(i);
//    human.GetVelocity().push_back(i);
//    human.GetVelocity().push_back(i);
//    human.GetVelocity().front() = i;
#endif
//    Object::ListType foo = human.GetVelocity();
//    cout<<foo.size()<<":"<<foo.front().AsFloat()<<","<<foo.back().AsFloat()<<endl;

    //move:
    Operation::Move move = Operation::Move::Instantiate();
    Object::ListType move_args;
    move_args.push_back(human.AsObject());
    move.SetArgs(move_args);
//    Object::MapType ent = move.GetArgs().front().AsMap();
//    cout<<"vel0:"<<ent["velocity"].AsList().front().AsFloat()<<endl;

    //sight:
    Operation::Sight sight = Operation::Sight::Instantiate();
    sight.SetFrom("123");
    Object::ListType sight_args;
    sight_args.push_back(move.AsObject());
    sight.SetArgs(sight_args);
//    Object::MapType ent = sight.GetArgs().front().AsMap()
//      ["args"].AsList().front().AsMap();
//    cout<<"vel0:"<<ent["velocity"].AsList().front().AsFloat()<<endl;
  }
  TIME_OFF("Plain creating of sight operation");

  NPC npc1;
  double x,y,z;
  TIME_ON;
  for(i=0; i<10000.0; i+=1.0) {
    //human:
    Entity::GameEntity human = Entity::GameEntity::Instantiate();
    human.SetId(npc1.GetId());
    Object::ListType vel;
    vel.push_back(i);
    vel.push_back(i-1.0);
    vel.push_back(i+1.0);
    human.SetVelocity(vel);

    //move:
    Operation::Move move = Operation::Move::Instantiate();
    Object::ListType move_args;
    move_args.push_back(human.AsObject());
    move.SetArgs(move_args);

    Operation::Sight res_sight = npc1.move(move);
    Object::ListType::iterator new_pos_i = res_sight.GetArgs().
      front().AsMap()["args"].AsList().
      front().AsMap()["pos"].AsList().begin();
    x = (*new_pos_i).AsFloat();
    new_pos_i++; y = (*new_pos_i).AsFloat();
    new_pos_i++; z = (*new_pos_i).AsFloat();
  }
  TIME_OFF("NPC movements");
  cout<<"Resulting position: ("<<x<<","<<y<<","<<z<<")"<<endl;

  return 0;
}
