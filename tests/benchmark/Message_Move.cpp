#include "timer.h"

#include <Atlas/Message/Element.h>

#include <iostream>
#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;

class NPC
{
public:
  NPC() : id("123") {x=y=z = vx=vy=vz = 0.0;}
  Element move(const Element &op);
  std::string getId() {return id;}
private:
  std::string id;
  double x,y,z;
  double vx,vy,vz;
};

Element NPC::move(const Element &op)
{
  ListType::iterator new_vel_i = ((Element&)op).asMap()["args"].asList().
    front().asMap()["velocity"].asList().begin();
  vx = (*new_vel_i).asFloat();
  new_vel_i++; vy = (*new_vel_i).asFloat();
  new_vel_i++; vz = (*new_vel_i).asFloat();

  x += vx;
  y += vy;
  z += vz;

  //human:
  MapType human;
  human["id"] = getId();
  ListType pos;
  pos.push_back(x);
  pos.push_back(y);
  pos.push_back(z);
  human["pos"] = pos;
  ListType vel;
  vel.push_back(vx);
  vel.push_back(vy);
  vel.push_back(vz);
  human["velocity"] = vel;
  
  //move:
  MapType move;
  move["objtype"] = Element("op");
  ListType move_parents(1, Element("move"));
  move["parents"] = move_parents;
  ListType move_args(1, human);
  move["args"] = move_args;
  
  //sight:
  MapType sight;
  sight["objtype"] = Element("op");
  ListType sight_parents(1, Element("sight"));
  sight["parents"] = sight_parents;
  sight["from"] = getId();
  ListType sight_args(1, move);
  sight["args"] = sight_args;

  return sight;
}

int main(int argc, char** argv)
{
  double i;
  Element bar(42);
  std::cout<<sizeof(bar)<<std::endl;
  TIME_ON;
  for(i=0; i<100000.0; i+=1.0) {
    //human:
    MapType human;
    human["objtype"] = Element("object");
    ListType human_parents(1, Element("human"));
    human["parents"] = human_parents;
    ListType pos;
    pos.push_back(i);
    pos.push_back(i-1.0);
    pos.push_back(i+1.0);
    human["pos"] = pos;
    ListType vel;
    vel.push_back(i);
    vel.push_back(i-1.0);
    vel.push_back(i+1.0);
    human["velocity"] = vel;

    //move:
    MapType move;
    move["objtype"] = Element("op");
    ListType move_parents(1, Element("move"));
    move["parents"] = move_parents;
    ListType move_args(1, human);
    move["args"] = move_args;

    //sight:
    MapType sight;
    sight["objtype"] = Element("op");
    ListType sight_parents(1, Element("sight"));
    sight["parents"] = sight_parents;
    sight["from"] = Element("123");
    ListType sight_args(1, move);
    sight["args"] = sight_args;

#if 0
    cout<<"----------------------------------------------------------------------"<<endl;
    cout<<"vel0:"<<sight["args"].asList().front().asMap()["args"].asList().front().asMap()["velocity"].asList().front().asFloat()<<endl;

    pos.front() = -i;
    cout<<"vel1:"<<sight["args"].asList().front().asMap()["args"].asList().front().asMap()["velocity"].asList().front().asFloat()<<endl;

    sight["args"].asList().front().asMap()["args"].asList().front().asMap()["velocity"].asList().front() = 2*i;
    cout<<"vel2:"<<sight["args"].asList().front().asMap()["args"].asList().front().asMap()["velocity"].asList().front().asFloat()<<endl;
    
    human["velocity"].asList().front() = -2*i;
    cout<<"vel3:"<<sight["args"].asList().front().asMap()["args"].asList().front().asMap()["velocity"].asList().front().asFloat()<<endl;
#endif
    
  }
  TIME_OFF("Plain creating of sight operation");


  NPC npc1;
  double x = 0., y = 0., z = 0.;
  TIME_ON;
  for(i=0; i<100000.0; i+=1.0) {
    //human:
    MapType human;
    human["id"] = npc1.getId();
    ListType vel;
    vel.push_back(i);
    vel.push_back(i-1.0);
    vel.push_back(i+1.0);
    human["velocity"] = vel;

    //move:
    MapType move;
    move["objtype"] = Element("op");
    ListType move_parents(1, Element("move"));
    move["parents"] = move_parents;
    ListType move_args(1, human);
    move["args"] = move_args;

    Element res_sight = npc1.move(move);
    ListType::iterator new_pos_i = res_sight.asMap()["args"].asList().
      front().asMap()["args"].asList().
      front().asMap()["pos"].asList().begin();
    x = (*new_pos_i).asFloat();
    new_pos_i++; y = (*new_pos_i).asFloat();
    new_pos_i++; z = (*new_pos_i).asFloat();
  }
  TIME_OFF("NPC movements");
  std::cout<<"Resulting position: ("<<x<<","<<y<<","<<z<<")"<<std::endl;

  return 0;
}
