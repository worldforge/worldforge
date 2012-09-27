#include "timer.h"

#include <iostream>
#include <cassert>

class Object {
public:
  int id;
  Object *parent;
  int objtype;
};

class Entity : public Object {
public:
  Object *loc;
  double pos[3];
  double velocity[3];
};

class Operation : public Object {
public:
  Object *from, *to;
  Object* arg;
};

enum {
  OBJECT,
  OP
};

Entity human;
Operation move;
Operation sight;

class NPC
{
public:
  NPC() : id(123) {x=y=z = vx=vy=vz = 0.0;}
  Operation *move(const Operation *op);
  int getId() {return id;}
private:
  int id;
  double x,y,z;
  double vx,vy,vz;
};

Operation *NPC::move(const Operation *op)
{
  double *new_vel = ((Entity*)op->arg)->velocity;
  vx = new_vel[0];
  vy = new_vel[1];
  vz = new_vel[2];

  x += vx;
  y += vy;
  z += vz;

  //human:
  static Entity human_ent;
  human_ent.parent = &human;
  human_ent.id = getId();
  human_ent.pos[0] = x;
  human_ent.pos[1] = y;
  human_ent.pos[2] = z;
  human_ent.velocity[0] = vx;
  human_ent.velocity[1] = vy;
  human_ent.velocity[2] = vz;
  
  //move:
  static Operation move_op;
  move_op.objtype = OP;
  move_op.parent = &::move;
  move_op.arg = &human_ent;
  
  //sight:
  static Operation sight_op;
  sight_op.objtype = OP;
  sight_op.parent = &sight;
  sight_op.from = &human_ent;
  sight_op.arg = &move_op;

  return &sight_op;
}

int main(int argc, char** argv)
{
  double i;
  TIME_ON;
  for(i=0; i<10000000.0; i+=1.0) {
    //human:
    Entity ent;
    ent.objtype=OBJECT;
    ent.parent=&human;
    ent.pos[0] = i;
    ent.pos[1] = i-1.0;
    ent.pos[2] = i+1.0;
    ent.velocity[0] = i;
    ent.velocity[1] = i-1.0;
    ent.velocity[2] = i+1.0;

    //move:
    Operation move_op;
    move_op.objtype=OP;
    move_op.parent=&move;
    move_op.arg=&ent;
    
    //sight:
    Operation sight_op;
    sight_op.objtype=OP;
    sight_op.parent=&sight;
    sight_op.from=&ent;
    sight_op.arg=&move_op;
  }
  TIME_OFF("Plain creating of sight operation");


  NPC npc1;
  double x,y,z;
  TIME_ON;
  for(i=0; i<10000000.0; i+=1.0) {
    //human:
    Entity human_ent;
    human_ent.parent = &human;
    human_ent.id = npc1.getId();
    human_ent.velocity[0] = i;
    human_ent.velocity[1] = i-1.0;
    human_ent.velocity[2] = i+1.0;

    //move:
    Operation move_op;
    move_op.objtype = OP;
    move_op.parent = &move;
    move_op.arg = &human_ent;

    Operation *res_sight = npc1.move(&move_op);
    Operation *res_move = (Operation*)(res_sight->arg);
    Entity *res_ent = (Entity*)res_move->arg;
    double *new_pos = res_ent->pos;
    x = new_pos[0];
    y = new_pos[1];
    z = new_pos[2];
  }
  TIME_OFF("NPC movements");
  std::cout<<"Resulting position: ("<<x<<","<<y<<","<<z<<")"<<std::endl;

  return 0;
}
