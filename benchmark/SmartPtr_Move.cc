#include <iostream>
#include <cassert>
#include <stdlib.h>

#include "timer.h"

using namespace std;

#define DEBUG 1

template <class T>
class FreeList
{
public:
  T data;
  FreeList() : count(1) { 
#if DEBUG
    cout << "FreeList(): this: " << this << endl; 
#endif
  }
  ~FreeList() {
#if DEBUG
    cout << "~FreeList(): this: " << this << endl;
#endif
    assert( count==0 );
  }
  unsigned GetRef() {
#if DEBUG
    cout << "FreeList.GetRef(): this: " << this << " count: " << count << " -> " << count+1 << endl;
#endif
    return ++count;
  }
  unsigned FreeRef() {
#if DEBUG
    cout << "FreeList.FreeRef(): this: " << this << " count: " << count << " -> " << count-1 << endl;
#endif
    assert( count > 0 );
    return --count;
  }
  void *operator new(size_t size) {
    if(begin) {
#if DEBUG
      cout << "FreeList.new: get from list: " << begin << endl;
#endif
      FreeList *res = begin;
      begin = begin->next;
      return res;
    }
#if DEBUG
    cout << "FreeList.new: malloc" << endl;
#endif
    return malloc(size);
  }
  void operator delete(void *_ptr) {
#if DEBUG
    cout << "FreeList.delete: _ptr: " << _ptr << endl;
#endif
    FreeList *ptr = (FreeList*)_ptr;
    ptr->next = begin;
    begin = ptr;
  }
private:
  int count;
  static FreeList *begin;
  FreeList *next;
};

class ObjectData {
public:
  int id;
  FreeList<ObjectData> *parent;
  int objtype;
};

FreeList<ObjectData> *FreeList<ObjectData>::begin = NULL;

class EntityData : public ObjectData {
public:
  FreeList<EntityData> *loc;
  double pos[3];
  double velocity[3];
};

FreeList<EntityData> *FreeList<EntityData>::begin = NULL;

class OperationData : public ObjectData {
public:
  FreeList<ObjectData> *from, *to;
  FreeList<ObjectData>* arg;
};

FreeList<OperationData> *FreeList<OperationData>::begin = NULL;

template <class T> 
class SmartPtr
{
public:
  FreeList<T> *ptr;
  SmartPtr() { 
    ptr = new FreeList<T>; 
#if DEBUG
    cout << "SmartPtr(): this: " << this << " ptr: " << ptr << endl;
#endif
  }
  SmartPtr(const SmartPtr<T>& a) {
#if DEBUG
    cout << "SmartPtr(): this: " << this << " ptr: " << ptr << " a:" << &a << " a.ptr: " << a.ptr << endl;
#endif
    ptr = a.get();
    IncRef();
  }
  ~SmartPtr() { 
#if DEBUG
    cout << "~SmartPtr(): this: " << this << " ptr: " << ptr << endl;
#endif
    DecRef();
  }
  void DecRef() {
    if( ! ptr->FreeRef() ) delete ptr;
  }
  void IncRef() {
    ptr->GetRef();
  }
  SmartPtr& operator=(const SmartPtr<T>& a) {
#if DEBUG
    cout << "SmartPtr.=: " << this << " ptr: " << ptr << " a:" << &a << " a.ptr: " << a.ptr << endl;
#endif
    if (a.get() != this->get()) {
#if DEBUG
      cout << "SmartPtr.=: yup, different" << endl;
#endif
      DecRef();
      ptr = a.get();
      IncRef();
    }
    return *this;
  }
  T& operator*() { 
    return ptr->data;
  }
  T* operator->() {
    return &ptr->data;
  }
  FreeList<T>* get() const {
    return ptr;
  }
};

typedef SmartPtr<ObjectData> Object;
typedef SmartPtr<EntityData> Entity;
typedef SmartPtr<OperationData> Operation;

enum {
  OBJECT,
  OP
};

Entity human;
//Operation move;
//Operation sight;

Entity& test(Entity &in)
{
  cout << "test: " << &in << endl;
  return in;
}

Entity& test2(Entity &in)
{
  cout << "test2: " << &in << endl;
  static Entity bar;
  bar = in;
  return in;
}

Entity& test3(Entity &in)
{
  cout << "test3: " << &in << endl;
  static Entity bar3(in);
  return in;
}

#if DEBUG
int main()
{
  cout << "Entity foo;" << endl;
  Entity foo;
  cout << "foo = human;" << endl;
  foo = human;
  cout << "foo = test(foo);" << endl;
  foo = test(foo);
  cout << "foo = test(human);" << endl;
  foo = test(human);
  cout << "foo = test2(foo);" << endl;
  foo = test2(foo);
  cout << "foo = test3(foo);" << endl;
  foo = test3(foo);
  cout << "...DONE" << endl;
  return 0;
}

#else
class NPC
{
public:
  NPC() : id(123) {x=y=z = vx=vy=vz = 0.0;}
  Operation move(const Operation op);
  int GetId() {return id;}
private:
  int id;
  double x,y,z;
  double vx,vy,vz;
};

Operation *NPC::move(const Operation op)
{
  double *new_vel = ((Entity)op->arg)->velocity;
  vx = new_vel[0];
  vy = new_vel[1];
  vz = new_vel[2];

  x += vx;
  y += vy;
  z += vz;

  //human:
  static Entity human_ent;
  human_ent.parent = &human;
  human_ent.id = GetId();
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
    move_op.parent=&sight;
    move_op.from=&ent;
    move_op.arg=&move_op;
  }
  TIME_OFF("Plain creating of sight operation");


  NPC npc1;
  double x,y,z;
  TIME_ON;
  for(i=0; i<10000000.0; i+=1.0) {
    //human:
    Entity human_ent;
    human_ent.parent = &human;
    human_ent.id = npc1.GetId();
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
  cout<<"Resulting position: ("<<x<<","<<y<<","<<z<<")"<<endl;

  return 0;
}
#endif
