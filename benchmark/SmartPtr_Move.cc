#include <iostream>
#include <cassert>
#include <stdlib.h>

#include "timer.h"

using namespace std;

class IncompatibleDataTypeException
{
public:
    IncompatibleDataTypeException(const int type1, const int type2) :
      data_type1(type1), data_type2(type2) {}
    int data_type1, data_type2;
};

#define DEBUG 1

template <class T>
class FreeList
{
public:
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
    cout << "FreeList.delete: _ptr: " << _ptr << " data_type: " << data_type << endl;
#endif
    FreeList *ptr = (FreeList*)_ptr;
    ptr->next = begin;
    begin = ptr;
  }
//private:
  static int data_type;
  int count;
  static FreeList *begin;
  FreeList *next;
  T data;
};

enum {
  OBJECT_DATA,
  ENTITY_DATA,
  OPERATION_DATA
};

class ObjectData {
public:
  ObjectData() : parent(NULL) { type = OBJECT_DATA; }
  int type;
  int id;
  FreeList<ObjectData> *parent;
  int objtype;
};

FreeList<ObjectData> *FreeList<ObjectData>::begin = NULL;
int FreeList<ObjectData>::data_type = OBJECT_DATA;

class EntityData : public ObjectData {
public:
  EntityData() : loc(NULL) { type = ENTITY_DATA; }
  FreeList<EntityData> *loc;
  double pos[3];
  double velocity[3];
};

FreeList<EntityData> *FreeList<EntityData>::begin = NULL;
int FreeList<EntityData>::data_type = ENTITY_DATA;

class OperationData : public ObjectData {
public:
  OperationData() : from(NULL), to(NULL), arg(NULL) { type = OPERATION_DATA; }
  FreeList<ObjectData> *from, *to;
  FreeList<ObjectData>* arg;
};

FreeList<OperationData> *FreeList<OperationData>::begin = NULL;
int FreeList<OperationData>::data_type = OPERATION_DATA;

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
    cout << "SmartPtr(): this: " << this << " ptr: " << ptr << " a: " << &a << " a.ptr: " << a.ptr << endl;
#endif
    ptr = a.get();
    IncRef();
  }
  // FreeList<ObjectData>* -> SmartPtr<EntityData>
  SmartPtr(const FreeList<ObjectData>*a_ptr)
    throw (IncompatibleDataTypeException) {
#if DEBUG
    cout << "SmartPtr(const FreeList<ObjectData>*a_ptr): this: " << this
         << "a_ptr: " << a_ptr 
         << " FreeList<T>::data_type: " << FreeList<T>::data_type
         << " a_ptr->data.type: " << a_ptr->data.type << endl;
#endif
    int type = FreeList<T>::data_type;
    if(type != a_ptr->data.type)
      throw IncompatibleDataTypeException(type, a_ptr->data.type);
    ptr = (FreeList<T>*)a_ptr;
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
  // SmartPtr<EntityData> -> FreeList<ObjectData>*
  FreeList<ObjectData>* AsObjectPtr() {
#if DEBUG
    cout << "AsObjectPtr(): this: " << this << " ptr: " << ptr << endl;
#endif
    //recipient should takes care of this: IncRef();
    return (FreeList<ObjectData>*)get();
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
Operation move;
Operation sight;

Entity test_cc(Entity in)
{
  cout << "test: " << &in << endl;
  return in;
}

Entity test_cr(Entity &in)
{
  cout << "test: " << &in << endl;
  return in;
}

Entity& test_rc(Entity in)
{
  cout << "test: " << &in << endl;
  return in;
}

Entity& test_rr(Entity &in)
{
  cout << "test: " << &in << endl;
  return in;
}

Entity test2(Entity &in)
{
  cout << "test2: " << &in << endl;
  static Entity bar;
  bar = in;
  return in;
}

Entity test3(Entity &in)
{
  cout << "test3: " << &in << endl;
  static Entity bar3(in);
  return in;
}

Entity test4(Entity &in)
{
  cout << "test4: " << &in << endl;
  Entity bar;
  bar = in;
  return bar;
}

#if DEBUG
int main()
{
  cout << endl << "Entity foo;" << endl;
  Entity foo;
  cout << endl << "foo = human;" << endl;
  foo = human;
  cout << endl << "foo = test_cc(foo);" << endl;
  foo = test_cc(foo);
  cout << endl << "foo = test_cr(foo);" << endl;
  foo = test_cr(foo);
  cout << endl << "foo = test_rc(foo);" << endl;
  foo = test_rc(foo);
  cout << endl << "foo = test_rr(foo);" << endl;
  foo = test_rr(foo);
  cout << endl << "foo = test(human);" << endl;
  foo = test_rr(human);
  cout << endl << "foo = test2(foo);" << endl;
  foo = test2(foo);
  cout << endl << "foo = test3(foo);" << endl;
  foo = test3(foo);
  cout << endl << "Entity foo4;" << endl;
  Entity foo4;
  cout << endl << "foo4 = test4(foo4);" << endl;
  foo4 = test4(foo4);
  cout << endl << "Entity from_obj(human.AsObjectPtr());" << endl;
  Entity from_obj(human.AsObjectPtr());
  cout << endl << "try {Operation op_from_obj(human.AsObjectPtr());}..." << endl;
  try {
    Operation op_from_obj(human.AsObjectPtr());
  }
  catch (IncompatibleDataTypeException e) {
    cout << "IncompatibleDataTypeException: " 
         << e.data_type1 << "!=" << e.data_type2 << endl;
  }
  cout << endl << "FreeList<ObjectData> *obj_ptr = human.AsObjectPtr();" << endl;
  FreeList<ObjectData> *obj_ptr = human.AsObjectPtr();
  cout << endl << "obj_ptr->GetRef();" << endl;
  obj_ptr->GetRef();
//  cout << endl << "delete obj_ptr;" << endl;
//  delete obj_ptr;
  cout << endl << "...DONE" << endl;
  return 0;
}

#else

class NPC
{
public:
  NPC() : id(123) {x=y=z = vx=vy=vz = 0.0;}
  Operation move(Operation &op);
  int GetId() {return id;}
private:
  int id;
  double x,y,z;
  double vx,vy,vz;
};

Operation NPC::move(Operation &op)
{
  Entity op_arg = op->arg;
  double *new_vel = op_arg->velocity;
  vx = new_vel[0];
  vy = new_vel[1];
  vz = new_vel[2];

  x += vx;
  y += vy;
  z += vz;

  //human:
  Entity human_ent;
  human_ent->parent = human.AsObjectPtr();
  human_ent->id = GetId();
  human_ent->pos[0] = x;
  human_ent->pos[1] = y;
  human_ent->pos[2] = z;
  human_ent->velocity[0] = vx;
  human_ent->velocity[1] = vy;
  human_ent->velocity[2] = vz;
  
  //move:
  Operation move_op;
  move_op->objtype = OP;
  move_op->parent = ::move.AsObjectPtr();
  move_op->arg = human_ent.AsObjectPtr();
  
  //sight:
  Operation sight_op;
  sight_op->objtype = OP;
  sight_op->parent = sight.AsObjectPtr();
  sight_op->from = human_ent.AsObjectPtr();
  sight_op->arg = move_op.AsObjectPtr();

  return sight_op;
}

int main(int argc, char** argv)
{
  double i;
  TIME_ON;
  for(i=0; i<10000000.0; i+=1.0) {
    //human:
    Entity ent;
    ent->objtype=OBJECT;
    ent->parent=human.AsObjectPtr();
    ent->pos[0] = i;
    ent->pos[1] = i-1.0;
    ent->pos[2] = i+1.0;
    ent->velocity[0] = i;
    ent->velocity[1] = i-1.0;
    ent->velocity[2] = i+1.0;

    //move:
    Operation move_op;
    move_op->objtype=OP;
    move_op->parent=move.AsObjectPtr();
    move_op->arg=ent.AsObjectPtr();
    
    //sight:
    Operation sight_op;
    sight_op->objtype=OP;
    move_op->parent=sight.AsObjectPtr();
    move_op->from=ent.AsObjectPtr();
    move_op->arg=move_op.AsObjectPtr();
  }
  TIME_OFF("Plain creating of sight operation");


  NPC npc1;
  double x,y,z;
  TIME_ON;
  for(i=0; i<10000000.0; i+=1.0) {
    //human:
    Entity human_ent;
    human_ent->parent = human.AsObjectPtr();
    human_ent->id = npc1.GetId();
    human_ent->velocity[0] = i;
    human_ent->velocity[1] = i-1.0;
    human_ent->velocity[2] = i+1.0;

    //move:
    Operation move_op;
    move_op->objtype = OP;
    move_op->parent = move.AsObjectPtr();
    move_op->arg = human_ent.AsObjectPtr();

    Operation res_sight = npc1.move(move_op);
    Operation res_move = res_sight->arg;
    Entity res_ent = res_move->arg;
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
