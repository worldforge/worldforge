#include "timer.h"

#include <Atlas/Message/Object.h>

#include <iostream>
#include <cassert>

using Atlas::Message::Element;
using namespace std;

class NPC
{
public:
  NPC() : id("123") {x=y=z = vx=vy=vz = 0.0;}
  Element move(const Element &op);
  string getId() {return id;}
private:
  string id;
  double x,y,z;
  double vx,vy,vz;
};

Element NPC::move(const Element &op)
{
  Element::ListType::iterator new_vel_i = ((Element&)op).asMap()["args"].asList().
    front().asMap()["velocity"].asList().begin();
  vx = (*new_vel_i).asFloat();
  new_vel_i++; vy = (*new_vel_i).asFloat();
  new_vel_i++; vz = (*new_vel_i).asFloat();

  x += vx;
  y += vy;
  z += vz;

  //human:
  Element::MapType human;
  human["id"] = getId();
  Element::ListType pos;
  pos.push_back(x);
  pos.push_back(y);
  pos.push_back(z);
  human["pos"] = pos;
  Element::ListType vel;
  vel.push_back(vx);
  vel.push_back(vy);
  vel.push_back(vz);
  human["velocity"] = vel;
  
  //move:
  Element::MapType move;
  move["objtype"] = Element("op");
  Element::ListType move_parents(1, Element("move"));
  move["parents"] = move_parents;
  Element::ListType move_args(1, human);
  move["args"] = move_args;
  
  //sight:
  Element::MapType sight;
  sight["objtype"] = Element("op");
  Element::ListType sight_parents(1, Element("sight"));
  sight["parents"] = sight_parents;
  sight["from"] = getId();
  Element::ListType sight_args(1, move);
  sight["args"] = sight_args;

  return sight;
}

int main(int argc, char** argv)
{
  double i;
  Element bar(42);
  cout<<sizeof(bar)<<endl;
  TIME_ON;
  for(i=0; i<100000.0; i+=1.0) {
    //human:
    Element::MapType human;
    human["objtype"] = Element("object");
    Element::ListType human_parents(1, Element("human"));
    human["parents"] = human_parents;
    Element::ListType pos;
    pos.push_back(i);
    pos.push_back(i-1.0);
    pos.push_back(i+1.0);
    human["pos"] = pos;
    Element::ListType vel;
    vel.push_back(i);
    vel.push_back(i-1.0);
    vel.push_back(i+1.0);
    human["velocity"] = vel;

    //move:
    Element::MapType move;
    move["objtype"] = Element("op");
    Element::ListType move_parents(1, Element("move"));
    move["parents"] = move_parents;
    Element::ListType move_args(1, human);
    move["args"] = move_args;

    //sight:
    Element::MapType sight;
    sight["objtype"] = Element("op");
    Element::ListType sight_parents(1, Element("sight"));
    sight["parents"] = sight_parents;
    sight["from"] = Element("123");
    Element::ListType sight_args(1, move);
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
    Element::MapType human;
    human["id"] = npc1.getId();
    Element::ListType vel;
    vel.push_back(i);
    vel.push_back(i-1.0);
    vel.push_back(i+1.0);
    human["velocity"] = vel;

    //move:
    Element::MapType move;
    move["objtype"] = Element("op");
    Element::ListType move_parents(1, Element("move"));
    move["parents"] = move_parents;
    Element::ListType move_args(1, human);
    move["args"] = move_args;

    Element res_sight = npc1.move(move);
    Element::ListType::iterator new_pos_i = res_sight.asMap()["args"].asList().
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
