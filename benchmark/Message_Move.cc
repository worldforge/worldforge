#include <iostream>
#include <cassert>
#include "../src/Message/Object.h"

#include "timer.h"

using namespace Atlas;
using namespace Atlas::Message;
using namespace std;

class NPC
{
public:
  NPC() : id("123") {x=y=z = vx=vy=vz = 0.0;}
  Object move(const Object &op);
  string GetId() {return id;}
private:
  string id;
  double x,y,z;
  double vx,vy,vz;
};

Object NPC::move(const Object &op)
{
  Object::ListType::iterator new_vel_i = ((Object&)op).AsMap()["args"].AsList().
    front().AsMap()["velocity"].AsList().begin();
  vx = (*new_vel_i).AsFloat();
  new_vel_i++; vy = (*new_vel_i).AsFloat();
  new_vel_i++; vz = (*new_vel_i).AsFloat();

  x += vx;
  y += vy;
  z += vz;

  //human:
  Object::MapType human;
  human["id"] = GetId();
  Object::ListType pos;
  pos.push_back(x);
  pos.push_back(y);
  pos.push_back(z);
  human["pos"] = pos;
  Object::ListType vel;
  vel.push_back(vx);
  vel.push_back(vy);
  vel.push_back(vz);
  human["velocity"] = vel;
  
  //move:
  Object::MapType move;
  move["objtype"] = Object("op");
  Object::ListType move_parents(1, Object("move"));
  move["parents"] = move_parents;
  Object::ListType move_args(1, human);
  move["args"] = move_args;
  
  //sight:
  Object::MapType sight;
  sight["objtype"] = Object("op");
  Object::ListType sight_parents(1, Object("sight"));
  sight["parents"] = sight_parents;
  sight["from"] = GetId();
  Object::ListType sight_args(1, move);
  sight["args"] = sight_args;

  return sight;
}

int main(int argc, char** argv)
{
  double i;
  Object bar(42);
  cout<<sizeof(bar)<<endl;
  TIME_ON;
  for(i=0; i<100000.0; i+=1.0) {
    //human:
    Object::MapType human;
    human["objtype"] = Object("object");
    Object::ListType human_parents(1, Object("human"));
    human["parents"] = human_parents;
    Object::ListType pos;
    pos.push_back(i);
    pos.push_back(i-1.0);
    pos.push_back(i+1.0);
    human["pos"] = pos;
    Object::ListType vel;
    vel.push_back(i);
    vel.push_back(i-1.0);
    vel.push_back(i+1.0);
    human["velocity"] = vel;

    //move:
    Object::MapType move;
    move["objtype"] = Object("op");
    Object::ListType move_parents(1, Object("move"));
    move["parents"] = move_parents;
    Object::ListType move_args(1, human);
    move["args"] = move_args;

    //sight:
    Object::MapType sight;
    sight["objtype"] = Object("op");
    Object::ListType sight_parents(1, Object("sight"));
    sight["parents"] = sight_parents;
    sight["from"] = Object("123");
    Object::ListType sight_args(1, move);
    sight["args"] = sight_args;

#if 0
    cout<<"----------------------------------------------------------------------"<<endl;
    cout<<"vel0:"<<sight["args"].AsList().front().AsMap()["args"].AsList().front().AsMap()["velocity"].AsList().front().AsFloat()<<endl;

    pos.front() = -i;
    cout<<"vel1:"<<sight["args"].AsList().front().AsMap()["args"].AsList().front().AsMap()["velocity"].AsList().front().AsFloat()<<endl;

    sight["args"].AsList().front().AsMap()["args"].AsList().front().AsMap()["velocity"].AsList().front() = 2*i;
    cout<<"vel2:"<<sight["args"].AsList().front().AsMap()["args"].AsList().front().AsMap()["velocity"].AsList().front().AsFloat()<<endl;
    
    human["velocity"].AsList().front() = -2*i;
    cout<<"vel3:"<<sight["args"].AsList().front().AsMap()["args"].AsList().front().AsMap()["velocity"].AsList().front().AsFloat()<<endl;
#endif
    
  }
  TIME_OFF("Plain creating of sight operation");


  NPC npc1;
  double x,y,z;
  TIME_ON;
  for(i=0; i<100000.0; i+=1.0) {
    //human:
    Object::MapType human;
    human["id"] = npc1.GetId();
    Object::ListType vel;
    vel.push_back(i);
    vel.push_back(i-1.0);
    vel.push_back(i+1.0);
    human["velocity"] = vel;

    //move:
    Object::MapType move;
    move["objtype"] = Object("op");
    Object::ListType move_parents(1, Object("move"));
    move["parents"] = move_parents;
    Object::ListType move_args(1, human);
    move["args"] = move_args;

    Object res_sight = npc1.move(move);
    Object::ListType::iterator new_pos_i = res_sight.AsMap()["args"].AsList().
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
