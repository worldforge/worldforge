#include "timer.h"

#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Entity/GameEntity.h>

#include <iostream>
#include <cassert>

using namespace Atlas;
using namespace Atlas::Objects;
using namespace Atlas::Message;
using namespace std;

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
  Object::ListType::iterator new_vel_i = op.getArgs().
    front().asMap()["velocity"].asList().begin();
  vx = (*new_vel_i).asFloat();
  new_vel_i++; vy = (*new_vel_i).asFloat();
  new_vel_i++; vz = (*new_vel_i).asFloat();

  x += vx;
  y += vy;
  z += vz;

  //human:
  GameEntity human = GameEntity::Instantiate();
  human.setId(getId());
  Object::ListType pos;
  pos.push_back(x);
  pos.push_back(y);
  pos.push_back(z);
  human.setPos(pos);
  Object::ListType vel;
  vel.push_back(vx);
  vel.push_back(vy);
  vel.push_back(vz);
  human.setVelocity(vel);

  //move:
  Move move = Move::Instantiate();
  Object::ListType move_args;
  move_args.push_back(human.asObject());
  move.setArgs(move_args);

  //sight:
  Sight sight = Sight::Instantiate();
  sight.setFrom(getId());
  Object::ListType sight_args;
  sight_args.push_back(move.asObject());
  sight.setArgs(sight_args);

  return sight;
}

int main(int argc, char** argv)
{
  double i;
  TIME_ON;
  for(i=0; i<10000.0; i+=1.0) {
    //human:
    GameEntity human = GameEntity::Instantiate();
    Object::ListType pos;
    pos.push_back(i);
    pos.push_back(i-1.0);
    pos.push_back(i+1.0);
    human.setPos(pos);
#if 1 //12.72user 0.00system 0:14.97elapsed 84%CPU
    Object::ListType vel;
    vel.push_back(i);
    vel.push_back(i-1.0);
    vel.push_back(i+1.0);
    human.setVelocity(vel);
#else //13.95user 0.00system 0:16.83elapsed 82%CPU
    Object::ListType::iterator veli = human.getVelocity().begin();
    *veli = i; veli++;
    *veli = i+1.0; veli++;
    *veli = i-1.0;
    
//    human.getVelocity().push_back(i);
//    human.getVelocity().push_back(i);
//    human.getVelocity().push_back(i);
//    human.getVelocity().front() = i;
#endif
//    Object::ListType foo = human.getVelocity();
//    cout<<foo.size()<<":"<<foo.front().asFloat()<<","<<foo.back().asFloat()<<endl;

    //move:
    Move move = Move::Instantiate();
    Object::ListType move_args;
    move_args.push_back(human.asObject());
    move.setArgs(move_args);
//    Object::MapType ent = move.getArgs().front().asMap();
//    cout<<"vel0:"<<ent["velocity"].asList().front().asFloat()<<endl;

    //sight:
    Sight sight = Sight::Instantiate();
    sight.setFrom("123");
    Object::ListType sight_args;
    sight_args.push_back(move.asObject());
    sight.setArgs(sight_args);
//    Object::MapType ent = sight.getArgs().front().asMap()
//      ["args"].asList().front().asMap();
//    cout<<"vel0:"<<ent["velocity"].asList().front().asFloat()<<endl;
  }
  TIME_OFF("Plain creating of sight operation");

  NPC npc1;
  double x,y,z;
  TIME_ON;
  for(i=0; i<10000.0; i+=1.0) {
    //human:
    GameEntity human = GameEntity::Instantiate();
    human.setId(npc1.getId());
    Object::ListType vel;
    vel.push_back(i);
    vel.push_back(i-1.0);
    vel.push_back(i+1.0);
    human.setVelocity(vel);

    //move:
    Move move = Move::Instantiate();
    Object::ListType move_args;
    move_args.push_back(human.asObject());
    move.setArgs(move_args);

    Sight res_sight = npc1.move(move);
    Object::ListType::iterator new_pos_i = res_sight.getArgs().
      front().asMap()["args"].asList().
      front().asMap()["pos"].asList().begin();
    x = (*new_pos_i).asFloat();
    new_pos_i++; y = (*new_pos_i).asFloat();
    new_pos_i++; z = (*new_pos_i).asFloat();
  }
  TIME_OFF("NPC movements");
  cout<<"Resulting position: ("<<x<<","<<y<<","<<z<<")"<<endl;

  return 0;
}
