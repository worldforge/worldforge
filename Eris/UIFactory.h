#ifndef ERIS_UI_FACTORY_H
#define ERIS_UI_FACTORY_H

#include <Atlas/Message/Element.h>

#include <sigc++/object.h>
#include <sigc++/signal.h>

#include <map>
#include <list>
#include <cassert>

namespace Atlas {
  namespace Objects {
    namespace Operation {
      class Create;
    }
    namespace Entity {
      class UIEntity;
      class Frame;
      class Slot;
    }
  }
}

namespace Eris { namespace UI {

// Element is a virtual class because we expect to
// get an inheritance hierarchy like:
//
//              Element
//              |     |
//    FrameElement    MyElement
//              |     |
//           MyFrameElement
//
// where MyElement and MyFrameElement are classes
// in client-side bindings. There should only be
// one Element instance in MyFrameElement, so
// we want children of Element to inherit
// virtually.

/// an instantiated dialog element
class Element
{
 public:
  virtual ~Element() {}

  // implementations of this function should emit PropertiesChanged for
  // valid properties
  virtual void setProperties(const Atlas::Message::Element::MapType&) = 0;

  void setProperty(const std::string& name, const Atlas::Message::Element& arg)
  {
    Atlas::Message::Element::MapType map;
    map[name] = arg;
    setProperties(map);
  }

  SigC::Signal1<void,const Atlas::Message::Element::MapType&> PropertiesChanged;
};

class SlotElement : public Element, virtual public SigC::Object
{
 public:
  virtual void action(const Atlas::Message::Element::MapType&) = 0;
};

class FrameElement : virtual public Element
{
 public:
  ~FrameElement()
  {
    for(SlotList::iterator I = _list.begin(); I!= _list.end(); ++I)
     delete *I;
  }

  virtual void pack(Element*) = 0;
  void packSlot(SlotElement* slot) {_list.push_back(slot);}

 private:
  typedef std::list<SlotElement*> SlotList;
  SlotList _list;
};

class Bindings;

class Factory
{
 public:
  class BaseGen
  {
   public:
    virtual ~BaseGen() {}
    virtual Element* create(const Atlas::Message::Element::MapType&) = 0;
  };

  template<class E>
  class Gen : public BaseGen
  {
   public:
    virtual Element* create(const Atlas::Message::Element::MapType& attrs)
		{return E(attrs);}
  };

  Factory(const std::string& id, BaseGen* gen)
	: _id(id), _gen(gen), _persistent(true), _refcount(1)
	{_id_list.push_back(id);}
  virtual ~Factory() {if(_persistent && _gen) delete _gen;}

  typedef std::map<std::string,Element*> IDMap;

  // create a dialog element
  virtual Element* create(IDMap&) const;
  // create a new factory
  virtual Factory* parse(const Atlas::Message::Element::MapType&, const Bindings&) const;

  void ref() {++_refcount;}
  void unref() {if(--_refcount == 0) delete this;}
  bool unique() const {return _refcount == 1;}
  bool persistent() const {return _persistent;}
  const std::string& id() const {return _id;}

 protected:
  typedef std::list<std::string> IDList;

  Factory(const Atlas::Objects::Entity::UIEntity&, const std::string&,
          const IDList&, const Atlas::Message::Element::MapType&, BaseGen*);

  const Atlas::Message::Element::MapType& attrs() const {return _attrs;}
  const IDList& idlist() const {return _id_list;}
  BaseGen* gen() const {return _gen;}

 private:
  Factory(const Factory&);
  Factory& operator=(const Factory&);

  std::string _id;
  IDList _id_list; // _id plus all parent ids, for use in connecting slots
  BaseGen* _gen;
  bool _persistent; // factories not created by the server
  unsigned long _refcount;
  Atlas::Message::Element::MapType _attrs;
};

// factory for children of Atlas::Objects::Entity::Frame
class FrameFactory : public Factory
{
 public:
  class BaseGen
  {
   public:
    virtual ~BaseGen() {}
    virtual FrameElement* create(const std::string& valign,
	const std::string& halign, const std::string& rel_pos,
	const Atlas::Message::Element::MapType&) = 0;
  };
  template<class FE>
  class Gen : public BaseGen
  {
   public:
    virtual FrameElement* create(const std::string& valign,
	const std::string& halign, const std::string& rel_pos,
	const Atlas::Message::Element::MapType& attrs)
		{return new FE(valign, halign, rel_pos, attrs);}
  };


  FrameFactory(BaseGen* gen) : Factory("frame", 0), _valign("center"),
	_halign("center"), _rel_pos("right"), _gen(gen) {}
  virtual ~FrameFactory();

  virtual Element* create(IDMap&) const;
  virtual Factory* parse(const Atlas::Message::Element::MapType&, const Bindings&) const;

 private:
  FrameFactory(const Atlas::Objects::Entity::Frame&, const IDList&,
	       const Bindings&, const Atlas::Message::Element::MapType&, BaseGen*);

  std::string _valign, _halign, _rel_pos;
  typedef std::list<Factory*> ChildList;
  ChildList _children;
  BaseGen* _gen;
};

class SlotFactory : public Factory
{
 public:
  // for the base slot classes
  SlotFactory(const std::string& id, BaseGen* gen) : Factory(id, gen) {}

  virtual Element* create(IDMap&) const;
  virtual Factory* parse(const Atlas::Message::Element::MapType&, const Bindings&) const;

  const Atlas::Message::Element::ListType& target() const {return _target;}

 private:
  SlotFactory(const Atlas::Objects::Entity::Slot&, const IDList&,
	      const Atlas::Message::Element::MapType&, BaseGen*);

  Atlas::Message::Element::ListType _target;
};

/// the dialog generator/handler
class Bindings
{
 public:
  Bindings();
  virtual ~Bindings();

  void parse(const Atlas::Message::Element&);

  /// purge all server-generated classes
  void clear();

  Factory* findFactory(const Atlas::Message::Element& id) const
  {
    return id.isString() ? findFactory(id.asString()) : 0;
  }

  Factory* findFactory(const std::string& id) const
  {
    FactoryMap::const_iterator I = _factory_map.find(id);
    return (I != _factory_map.end()) ? I->second : 0;
  }

  bool bind(Factory* f) {return _factory_map.insert(
	FactoryMap::value_type(f->id(), f)).second;}

  // a trick to get reasonable syntax for the interface,
  // e.g. Bind<Foo>(bindings).slot("foo");
  template<class C>
  friend struct Bind
  {
   public:
    Bind(Bindings& b) : _b(b) {}

    bool slot(const std::string& id)
	{return _b.bind(new SlotFactory(id, new SlotFactory::Gen<C>()));}
    bool element(const std::string& id)
	{return _b.bind(new Factory(id, new Factory::Gen<C>()));}
    bool frame()
	{return _b.bind(new FrameFactory(new FrameFactory::Gen<C>()));}
   private:
    Bindings& _b;
  };

  // put an Element on the screen as a dialog
  virtual void display(Element*, const std::string& id) = 0;

  virtual void createConsoleElement(const Atlas::Objects::Entity::UIEntity&) = 0;

 private:

 typedef std::map<std::string,Factory*> FactoryMap;
 FactoryMap _factory_map;
};

}} // namespace Eris::UI

#endif
