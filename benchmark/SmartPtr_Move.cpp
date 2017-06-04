#include "timer.h"

#include <iostream>
#include <cassert>
#include <stdlib.h>

using namespace std;

class IncompatibleDataTypeException
{
public:
    IncompatibleDataTypeException(const int type1, const int type2) :
      data_type1(type1), data_type2(type2) {}
    int data_type1, data_type2;
};

//#undef assert
//#define assert(expr)		((void) 0)
#undef DEBUG
#define DEBUG 0
#if DEBUG
#define MAX_ITER 10.0
#else
#define MAX_ITER 10000000.0
#endif
#define FINE_GRAINED_LISTS 1

template <class T>
class FreeList
{
public:
  FreeList() : count(0) { 
#if DEBUG
    cout << "FreeList(): this: " << this << " data_type: " << data_type << endl; 
#endif
  }
  ~FreeList() {
#if DEBUG
    cout << "~FreeList(): this: " << this << endl;
#endif
    assert( count==0 );
  }
  static FreeList<T> *alloc() {
    if(begin) {
#if DEBUG
      cout << "FreeList.alloc(): get from list: " << begin << endl;
#endif
      FreeList *res = begin;
      assert( res->count == 0 );
      //res->incRef();
      begin = begin->next;
      return res;
    }
#if DEBUG
    cout << "FreeList.alloc(): new" << endl;
#endif
    return new FreeList<T>;
  }
  
  void free() {
#if DEBUG
    cout << "FreeList.free(): this: " << this << " data_type: " << data_type << endl;
#endif
    //data.freeObjectPtr();
    next = begin;
    begin = this;
  }

  void incRef() {
#if DEBUG
    cout << "FreeList.incRef(): this: " << this << " count: " << count << " -> " << count+1 << endl;
#endif
    count++;
  }
  void decRef() {
#if DEBUG
    cout << "FreeList.decRef(): this: " << this << " count: " << count << " -> " << count-1 << endl;
#endif
    assert( count >= 0 );
    if(!count) {
      free();
      return;
    }
    count--;
  }
//private:
  static int data_type;
  int count;
  static FreeList *begin;
  FreeList *next;
  T data;
};

class ObjectData;
inline void decRef(FreeList<ObjectData> *ptr);

enum {
  OBJECT_DATA,
  ENTITY_DATA,
  OPERATION_DATA
};

class ObjectData {
public:
  ObjectData() : parent(NULL) { type = OBJECT_DATA; }
#define freeFoo(a) if(a) { decRef(a); a = NULL; }
  ~ObjectData() { freeObjectPtr(); }
  void freeObjectPtr() {
    freeFoo(parent);
  }
  int type;
  int id;
#define setFoo(a)                         \
  void set_##a(FreeList<ObjectData> *p) { \
    if(a!=p) {                            \
      if(a) ::decRef(a);                  \
      if(p) p->incRef();                  \
      a = p;                              \
    }                                     \
  }
  setFoo(parent);
  FreeList<ObjectData> *parent;
  int objtype;
};

FreeList<ObjectData> *FreeList<ObjectData>::begin = NULL;
int FreeList<ObjectData>::data_type = OBJECT_DATA;

class EntityData : public ObjectData {
public:
  EntityData() : loc(NULL) { type = ENTITY_DATA; }
  void freeObjectPtr() {
    ObjectData::freeObjectPtr();
    freeFoo(loc);
  }
  setFoo(loc);
  FreeList<ObjectData> *loc;
  double pos[3];
  double velocity[3];
};

FreeList<EntityData> *FreeList<EntityData>::begin = NULL;
int FreeList<EntityData>::data_type = ENTITY_DATA;

class OperationData : public ObjectData {
public:
  OperationData() : from(NULL), to(NULL), arg(NULL) { type = OPERATION_DATA; }
  void freeObjectPtr() {
    ObjectData::freeObjectPtr();
    freeFoo(from);
    freeFoo(to);
    freeFoo(arg);
  }
  setFoo(from);
  setFoo(to);
  setFoo(arg);
  FreeList<ObjectData> *from, *to;
  FreeList<ObjectData> *arg;
};

FreeList<OperationData> *FreeList<OperationData>::begin = NULL;
int FreeList<OperationData>::data_type = OPERATION_DATA;

inline void decRef(FreeList<ObjectData> *ptr)
{
  switch(ptr->data.type) {
  case OBJECT_DATA:
    ptr->decRef();
    break;
  case ENTITY_DATA:
    ((FreeList<EntityData> *)ptr)->decRef();
    break;
  case OPERATION_DATA:
    ((FreeList<OperationData> *)ptr)->decRef();
    break;
  default:
    cout << "Unknown object class: " << ptr->data.type;
  }
}

template <class T> 
class SmartPtr
{
public:
  FreeList<T> *ptr;
  SmartPtr() { 
    ptr = FreeList<T>::alloc(); 
#if DEBUG
    cout << "SmartPtr(): this: " << this << " ptr: " << ptr << endl;
#endif
  }
  SmartPtr(const SmartPtr<T>& a) {
#if DEBUG
    cout << "SmartPtr(): this: " << this << " ptr: " << ptr << " a: " << &a << " a.ptr: " << a.ptr << endl;
#endif
    ptr = a.get();
    incRef();
  }
  // FreeList<ObjectData>* -> SmartPtr<EntityData>
  SmartPtr(const FreeList<ObjectData>*a_ptr)
    {
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
    incRef();
  }
  ~SmartPtr() { 
#if DEBUG
    cout << "~SmartPtr(): this: " << this << " ptr: " << ptr << endl;
#endif
    decRef();
  }
  void decRef() {
    ptr->decRef();
  }
  void incRef() {
    ptr->incRef();
  }
  // SmartPtr<EntityData> -> FreeList<ObjectData>*
  FreeList<ObjectData>* asObjectPtr() {
#if DEBUG
    cout << "asObjectPtr(): this: " << this << " ptr: " << ptr << endl;
#endif
    //recipient should takes care of this: incRef();
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
      decRef();
      ptr = a.get();
      incRef();
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

FreeList<ObjectData> *test_obj_ptr()
{
  Operation ent;
  ent.incRef();
  return ent.asObjectPtr();
}

#if DEBUG==2
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
  cout << endl << "Entity from_obj(human.asObjectPtr());" << endl;
  Entity from_obj(human.asObjectPtr());
  cout << endl << "try {Operation op_from_obj(human.asObjectPtr());}..." << endl;
  try {
    Operation op_from_obj(human.asObjectPtr());
  }
  catch (IncompatibleDataTypeException e) {
    cout << "IncompatibleDataTypeException: " 
         << e.data_type1 << "!=" << e.data_type2 << endl;
  }
  cout << endl << "FreeList<ObjectData> *obj_ptr = test_obj_ptr();" << endl;
  FreeList<ObjectData> *obj_ptr = test_obj_ptr();
  cout << endl << "decRef(obj_ptr);" << endl;
  decRef(obj_ptr);
  cout << endl << "...DONE" << endl;
  return 0;
}

#else

class NPC
{
public:
  NPC() : id(123) {x=y=z = vx=vy=vz = 0.0;}
  Operation move(Operation &op);
  int getId() {return id;}
private:
  int id;
  double x,y,z;
  double vx,vy,vz;
};

Operation NPC::move(Operation &op)
{
#if DEBUG
  cout << endl << "DEBUG: " << __LINE__ << ":  Entity op_arg = op->arg;" << endl;
#endif
  Entity op_arg = op->arg;
  double *new_vel = op_arg->velocity;
  vx = new_vel[0];
  vy = new_vel[1];
  vz = new_vel[2];

  x += vx;
  y += vy;
  z += vz;

  //human:
#if DEBUG
  cout << endl << "DEBUG: " << __LINE__ << ":  Entity human_ent;" << endl;
#endif
  Entity human_ent;
#if !FINE_GRAINED_LISTS
#if DEBUG
  cout << endl << "DEBUG: " << __LINE__ << ":  human_ent->set_parent(human.asObjectPtr());" << endl;
#endif
  human_ent->set_parent(human.asObjectPtr());
#endif // !FINE_GRAINED_LISTS
  human_ent->id = getId();
  human_ent->pos[0] = x;
  human_ent->pos[1] = y;
  human_ent->pos[2] = z;
  human_ent->velocity[0] = vx;
  human_ent->velocity[1] = vy;
  human_ent->velocity[2] = vz;
  
  //move:
#if DEBUG
  cout << endl << "DEBUG: " << __LINE__ << ":  Operation move_op;" << endl;
#endif
  Operation move_op;
#if !FINE_GRAINED_LISTS
  move_op->objtype = OP;
#if DEBUG
  cout << endl << "DEBUG: " << __LINE__ << ":  move_op->set_parent(::move.asObjectPtr());" << endl;
#endif
  move_op->set_parent(::move.asObjectPtr());
#endif // !FINE_GRAINED_LISTS
#if DEBUG
  cout << endl << "DEBUG: " << __LINE__ << ":  move_op->set_arg(human_ent.asObjectPtr());" << endl;
#endif
  move_op->set_arg(human_ent.asObjectPtr());
  
  //sight:
#if DEBUG
  cout << endl << "DEBUG: " << __LINE__ << ":  Operation sight_op;" << endl;
#endif
  Operation sight_op;
#if !FINE_GRAINED_LISTS
  sight_op->objtype = OP;
#if DEBUG
  cout << endl << "DEBUG: " << __LINE__ << ":  sight_op->set_parent(sight.asObjectPtr());" << endl;
#endif
  sight_op->set_parent(sight.asObjectPtr());
#endif // !FINE_GRAINED_LISTS
#if DEBUG
  cout << endl << "DEBUG: " << __LINE__ << ":  sight_op->set_from(human_ent.asObjectPtr());" << endl;
#endif
  sight_op->set_from(human_ent.asObjectPtr());
#if DEBUG
  cout << endl << "DEBUG: " << __LINE__ << ":  sight_op->set_arg(move_op.asObjectPtr());" << endl;
#endif
  sight_op->set_arg(move_op.asObjectPtr());

  return sight_op;
}

int main(int argc, char** argv)
{
  double i;
  TIME_ON;
  for(i=0; i<MAX_ITER; i+=1.0) {
    //human:
#if DEBUG
    cout << endl << "DEBUG: " << __LINE__ << ":    Entity ent;" << endl;
#endif
    Entity ent;
#if !FINE_GRAINED_LISTS
    ent->objtype=OBJECT;
#if DEBUG
    cout << endl << "DEBUG: " << __LINE__ << ":    ent->set_parent(human.asObjectPtr());" << endl;
#endif
    ent->set_parent(human.asObjectPtr());
#endif // !FINE_GRAINED_LISTS
    ent->pos[0] = i;
    ent->pos[1] = i-1.0;
    ent->pos[2] = i+1.0;
    ent->velocity[0] = i;
    ent->velocity[1] = i-1.0;
    ent->velocity[2] = i+1.0;

    //move:
#if DEBUG
    cout << endl << "DEBUG: " << __LINE__ << ":    Operation move_op;" << endl;
#endif
    Operation move_op;
#if !FINE_GRAINED_LISTS
    move_op->objtype=OP;
#if DEBUG
    cout << endl << "DEBUG: " << __LINE__ << ":    move_op->set_parent(move.asObjectPtr());" << endl;
#endif
    move_op->set_parent(move.asObjectPtr());
#endif // !FINE_GRAINED_LISTS
#if DEBUG
    cout << endl << "DEBUG: " << __LINE__ << ":    move_op->set_arg(ent.asObjectPtr());" << endl;
#endif
    move_op->set_arg(ent.asObjectPtr());
    
    //sight:
#if DEBUG
    cout << endl << "DEBUG: " << __LINE__ << ":    Operation sight_op;" << endl;
#endif
    Operation sight_op;
#if !FINE_GRAINED_LISTS
    sight_op->objtype=OP;
#if DEBUG
    cout << endl << "DEBUG: " << __LINE__ << ":    sight_op->set_parent(sight.asObjectPtr());" << endl;
#endif
    sight_op->set_parent(sight.asObjectPtr());
#endif // !FINE_GRAINED_LISTS
#if DEBUG
    cout << endl << "DEBUG: " << __LINE__ << ":    sight_op->set_from(ent.asObjectPtr());" << endl;
#endif
    sight_op->set_from(ent.asObjectPtr());
#if DEBUG
    cout << endl << "DEBUG: " << __LINE__ << ":    sight_op->set_arg(move_op.asObjectPtr());" << endl;
#endif
    sight_op->set_arg(move_op.asObjectPtr());
#if DEBUG
    cout << endl << "DEBUG: " << __LINE__ << ":    DONE iter" << endl;
#endif
  }
  TIME_OFF("Plain creating of sight operation");


#if DEBUG
  cout << endl << "DEBUG: " << __LINE__ << ":  NPC npc1;" << endl;
#endif
  NPC npc1;
  double x,y,z;
  TIME_ON;
  for(i=0; i<MAX_ITER; i+=1.0) {
    //human:
#if DEBUG
    cout << endl << "DEBUG: " << __LINE__ << ":    Entity human_ent;" << endl;
#endif
    Entity human_ent;
#if DEBUG
    cout << endl << "DEBUG: " << __LINE__ << ":    human_ent->set_parent(human.asObjectPtr());" << endl;
#endif
#if !FINE_GRAINED_LISTS
    human_ent->set_parent(human.asObjectPtr());
#endif // !FINE_GRAINED_LISTS
    human_ent->id = npc1.getId();
    human_ent->velocity[0] = i;
    human_ent->velocity[1] = i-1.0;
    human_ent->velocity[2] = i+1.0;

    //move:
#if DEBUG
    cout << endl << "DEBUG: " << __LINE__ << ":    Operation move_op;" << endl;
#endif
    Operation move_op;
#if !FINE_GRAINED_LISTS
    move_op->objtype = OP;
#if DEBUG
    cout << endl << "DEBUG: " << __LINE__ << ":    move_op->set_parent(move.asObjectPtr());" << endl;
#endif
    move_op->set_parent(move.asObjectPtr());
#endif // !FINE_GRAINED_LISTS
#if DEBUG
    cout << endl << "DEBUG: " << __LINE__ << ":    move_op->set_arg(human_ent.asObjectPtr());" << endl;
#endif
    move_op->set_arg(human_ent.asObjectPtr());

#if DEBUG
    cout << endl << "DEBUG: " << __LINE__ << ":    Operation res_sight = npc1.move(move_op);" << endl;
#endif
    Operation res_sight = npc1.move(move_op);
#if DEBUG
    cout << endl << "DEBUG: " << __LINE__ << ":    Operation res_move = res_sight->arg;" << endl;
#endif
    Operation res_move = res_sight->arg;
#if DEBUG
    cout << endl << "DEBUG: " << __LINE__ << ":    Entity res_ent = res_move->arg;" << endl;
#endif
    Entity res_ent = res_move->arg;
    double *new_pos = res_ent->pos;
    x = new_pos[0];
    y = new_pos[1];
    z = new_pos[2];
#if DEBUG
    cout << endl << "DEBUG: " << __LINE__ << ":    DONE iter" << endl;
#endif
  }
  TIME_OFF("NPC movements");
  cout<<"Resulting position: ("<<x<<","<<y<<","<<z<<")"<<endl;

  return 0;
}
#endif
