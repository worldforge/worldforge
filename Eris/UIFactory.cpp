#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <Eris/UIFactory.h>

#include <Atlas/Objects/Entity/UIEntity.h>
#include <Atlas/Objects/Entity/Frame.h>
#include <Atlas/Objects/Entity/Slot.h>

#include <sigc++/object_slot.h>

Eris::UI::Factory::Factory(const Atlas::Objects::Entity::UIEntity& entity,
			   const std::string& base_type,
			   const IDList& parents,
		           const Atlas::Message::Object::MapType& attrs,
			   BaseGen* gen) :
	_id(entity.GetId()), _id_list(parents), _gen(gen),
	_persistent(false), _refcount(1), _attrs(attrs)
{
  _id_list.push_back(_id);
  _attrs["display_status"] = entity.GetDisplayStatus();
  Atlas::Objects::Root::const_iterator I, last(entity, base_type);
  for(I = entity.begin(); I != last; ++I)
    _attrs[I->first] = I->second;
}

Eris::UI::Element*
Eris::UI::Factory::create(IDMap& idmap) const
{
  Element* elem = _gen->create(attrs());
  for(IDList::const_iterator I = idlist().begin(); I != idlist().end(); ++I)
    idmap[*I] = elem;
  return elem;
}

Eris::UI::Factory*
Eris::UI::Factory::parse(const Atlas::Message::Object::MapType& map,
			 const Bindings&) const
{
  Atlas::Objects::Entity::UIEntity entity;

  // FIXME isn't there a better way to do this?
  Atlas::Message::Object::MapType::const_iterator I = map.begin();
  while(I != map.end())
    entity.SetAttr(I->first, I->second);

  return new Factory(entity, "u_i_entity", idlist(), attrs(), _gen);
}

Eris::UI::FrameFactory::~FrameFactory()
{
  for(ChildList::iterator I = _children.begin(); I != _children.end(); ++I)
    (*I)->unref();
  if(persistent()) // original frame factory, owns BaseGen
    delete _gen;
}

Eris::UI::Element*
Eris::UI::FrameFactory::create(IDMap& idmap) const
{
  FrameElement* frame = _gen->create(_valign, _halign, _rel_pos, attrs());

  // we don't want child slots connecting to anything
  // outside this frame, so we use a local idmap

  IDMap local;

  for(IDList::const_iterator I = idlist().begin(); I != idlist().end(); ++I)
    local[*I] = frame;

  for(ChildList::const_iterator I = _children.begin(); I != _children.end(); ++I) {
    Element* child = (*I)->create(local);
    SlotElement* slot = dynamic_cast<SlotElement*>(child);
    if(slot)
      frame->packSlot(slot);
    else
      frame->pack(child);
  }

  // FIXME if one insertion fails, do they all fail?
  idmap.insert(local.begin(), local.end());

  return frame;
}

Eris::UI::Factory*
Eris::UI::FrameFactory::parse(const Atlas::Message::Object::MapType& map,
			      const Bindings& factories) const
{
  Atlas::Objects::Entity::Frame frame;

  // FIXME isn't there a better way to do this?
  Atlas::Message::Object::MapType::const_iterator I = map.begin();
  while(I != map.end())
    frame.SetAttr(I->first, I->second);

  FrameFactory* factory = new FrameFactory(frame, idlist(), factories, attrs(), _gen);

  // if 'contains' isn't set, then we're dealing with a child
  // of Frame being used as an opaque type, so we keep the same
  // children
  if(factory->_children.empty()) {
    factory->_valign = _valign;
    factory->_halign = _halign;
    factory->_rel_pos = _rel_pos;
    factory->_children = _children;
    for(ChildList::const_iterator I = _children.begin(); I != _children.end(); ++I)
      (*I)->ref();
    return factory;
  }

  return factory;
}

Eris::UI::FrameFactory::FrameFactory(const Atlas::Objects::Entity::Frame& frame,
				     const IDList& parents, const Bindings& factories,
			             const Atlas::Message::Object::MapType& attrs,
				     BaseGen* gen)
	: Factory(frame, "frame", parents, attrs, 0),
	  _valign(frame.GetValign()), _halign(frame.GetHalign()),
	  _rel_pos(frame.GetRelPos()), _gen(gen)
{
  const Atlas::Message::Object::ListType& contains = frame.GetContains();

  Atlas::Message::Object::ListType::const_iterator I;

  for(I = contains.begin(); I != contains.end(); ++I) {
    Factory* child = factories.findFactory(*I);
    if(child) {
      _children.push_back(child);
      child->ref();
    }
  }
}

Eris::UI::SlotFactory::SlotFactory(const Atlas::Objects::Entity::Slot& slot,
				   const IDList& parents,
				   const Atlas::Message::Object::MapType& attrs,
				   BaseGen* gen)
	: Factory(slot, "slot", parents, attrs, gen), _target(slot.GetTarget())
{

}

Eris::UI::Element*
Eris::UI::SlotFactory::create(IDMap& idmap) const
{
  SlotElement* slot = dynamic_cast<SlotElement*>(gen()->create(attrs()));
  assert(slot);
  Atlas::Message::Object::ListType::const_iterator T;
  for(T = _target.begin(); T != _target.end(); ++T) {
    if(!T->IsString())
      continue;
    IDMap::iterator I = idmap.find(T->AsString());
    if(I != idmap.end())
      I->second->PropertiesChanged.connect(SigC::slot(*slot, &SlotElement::action));
  }
  for(IDList::const_iterator I = idlist().begin(); I != idlist().end(); ++I)
    idmap[*I] = slot;
  return slot;
}

Eris::UI::Factory*
Eris::UI::SlotFactory::parse(const Atlas::Message::Object::MapType& map,
			     const Bindings&) const
{
  Atlas::Objects::Entity::Slot slot;

  // FIXME isn't there a better way to do this?
  Atlas::Message::Object::MapType::const_iterator I = map.begin();
  while(I != map.end())
    slot.SetAttr(I->first, I->second);

  if(slot.GetTarget().empty())
    return 0;

  return new SlotFactory(slot, idlist(), attrs(), gen());
}

Eris::UI::Bindings::Bindings()
{
  // FIXME bind slot factories here
}

Eris::UI::Bindings::~Bindings()
{
  for(FactoryMap::iterator I = _factory_map.begin(); I != _factory_map.end(); ++I)
    I->second->unref();
}

void
Eris::UI::Bindings::parse(const Atlas::Message::Object& obj)
{
  if(!obj.IsMap())
    return;

  const Atlas::Message::Object::MapType& map = obj.AsMap();

  Atlas::Message::Object::MapType::const_iterator J = map.find("display_status");

  if(J != map.end() && J->second.IsString() && J->second.AsString() == "console") {
    Atlas::Objects::Entity::UIEntity entity;

    // FIXME isn't there a better way to do this?
    Atlas::Message::Object::MapType::const_iterator I = map.begin();
    while(I != map.end())
      entity.SetAttr(I->first, I->second);

    createConsoleElement(entity);
    return;
  }

  Atlas::Message::Object::MapType::const_iterator parents = map.find("parents");

  if(parents == map.end() || !parents->second.IsList())
    return;

  J = map.find("objtype");

  if(J == map.end() || !J->second.IsString())
    return;

  const std::string& objtype = J->second.AsString();

  if(objtype != "class" && objtype != "object")
    return;

  Factory* factory = 0;

  Atlas::Message::Object::ListType::const_iterator I;
  for(I = parents->second.AsList().begin(); I != parents->second.AsList().end(); ++I) {
    Factory* parent = findFactory(*I);
    if(parent) {
      factory = parent->parse(obj.AsMap(), *this);
      break;
    }
  }

  if(!factory)
    return;

  if(objtype == "class") {
    if(!bind(factory))
      factory->unref();
  }
  else { // objtype == "object"
    Factory::IDMap idmap;
    display(factory->create(idmap), factory->id());
    factory->unref();
  }
}

void
Eris::UI::Bindings::clear()
{
  FactoryMap keepers;

  for(FactoryMap::iterator I = _factory_map.begin(); I != _factory_map.end(); ++I) {
    if(I->second->persistent())
      keepers.insert(*I);
    else
      I->second->unref();
  }

  _factory_map = keepers;
}
