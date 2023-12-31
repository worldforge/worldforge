// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubAtlasProperties_custom.h file.

#ifndef STUB_RULES_ATLASPROPERTIES_H
#define STUB_RULES_ATLASPROPERTIES_H

#include "rules/AtlasProperties.h"
#include "stubAtlasProperties_custom.h"

#ifndef STUB_IdProperty_IdProperty
//#define STUB_IdProperty_IdProperty
   IdProperty::IdProperty(const std::string& data)
    : PropertyBase(data)
  {
    
  }
#endif //STUB_IdProperty_IdProperty

#ifndef STUB_IdProperty_get
//#define STUB_IdProperty_get
  int IdProperty::get(Atlas::Message::Element& val) const
  {
    return 0;
  }
#endif //STUB_IdProperty_get

#ifndef STUB_IdProperty_set
//#define STUB_IdProperty_set
  void IdProperty::set(const Atlas::Message::Element& val)
  {
    
  }
#endif //STUB_IdProperty_set

#ifndef STUB_IdProperty_add
//#define STUB_IdProperty_add
  void IdProperty::add(const std::string& key, Atlas::Message::MapType& map) const
  {
    
  }
#endif //STUB_IdProperty_add

#ifndef STUB_IdProperty_add
//#define STUB_IdProperty_add
  void IdProperty::add(const std::string& key, const Atlas::Objects::Entity::RootEntity& ent) const
  {
    
  }
#endif //STUB_IdProperty_add

#ifndef STUB_IdProperty_copy
//#define STUB_IdProperty_copy
  IdProperty* IdProperty::copy() const
  {
    return nullptr;
  }
#endif //STUB_IdProperty_copy


#ifndef STUB_LocationProperty_LocationProperty
//#define STUB_LocationProperty_LocationProperty
   LocationProperty::LocationProperty(const LocatedEntity& entity)
    : PropertyBase(entity)
  {
    
  }
#endif //STUB_LocationProperty_LocationProperty

#ifndef STUB_LocationProperty_get
//#define STUB_LocationProperty_get
  int LocationProperty::get(Atlas::Message::Element& val) const
  {
    return 0;
  }
#endif //STUB_LocationProperty_get

#ifndef STUB_LocationProperty_set
//#define STUB_LocationProperty_set
  void LocationProperty::set(const Atlas::Message::Element& val)
  {
    
  }
#endif //STUB_LocationProperty_set

#ifndef STUB_LocationProperty_add
//#define STUB_LocationProperty_add
  void LocationProperty::add(const std::string& key, Atlas::Message::MapType& map) const
  {
    
  }
#endif //STUB_LocationProperty_add

#ifndef STUB_LocationProperty_add
//#define STUB_LocationProperty_add
  void LocationProperty::add(const std::string& key, const Atlas::Objects::Entity::RootEntity& ent) const
  {
    
  }
#endif //STUB_LocationProperty_add

#ifndef STUB_LocationProperty_copy
//#define STUB_LocationProperty_copy
  LocationProperty* LocationProperty::copy() const
  {
    return nullptr;
  }
#endif //STUB_LocationProperty_copy

#ifndef STUB_LocationProperty_data
//#define STUB_LocationProperty_data
  const std::string& LocationProperty::data() const
  {
    static std::string _static_instance; return _static_instance;
  }
#endif //STUB_LocationProperty_data


#ifndef STUB_NameProperty_NameProperty
//#define STUB_NameProperty_NameProperty
   NameProperty::NameProperty(unsigned int flags)
    : Property(flags)
  {
    
  }
#endif //STUB_NameProperty_NameProperty

#ifndef STUB_NameProperty_add
//#define STUB_NameProperty_add
  void NameProperty::add(const std::string& key, const Atlas::Objects::Entity::RootEntity& ent) const
  {
    
  }
#endif //STUB_NameProperty_add


#ifndef STUB_ContainsProperty_ContainsProperty
//#define STUB_ContainsProperty_ContainsProperty
   ContainsProperty::ContainsProperty(LocatedEntitySet& data)
    : PropertyBase(data)
  {
    
  }
#endif //STUB_ContainsProperty_ContainsProperty

#ifndef STUB_ContainsProperty_get
//#define STUB_ContainsProperty_get
  int ContainsProperty::get(Atlas::Message::Element& val) const
  {
    return 0;
  }
#endif //STUB_ContainsProperty_get

#ifndef STUB_ContainsProperty_set
//#define STUB_ContainsProperty_set
  void ContainsProperty::set(const Atlas::Message::Element& val)
  {
    
  }
#endif //STUB_ContainsProperty_set

#ifndef STUB_ContainsProperty_add
//#define STUB_ContainsProperty_add
  void ContainsProperty::add(const std::string& key, const Atlas::Objects::Entity::RootEntity& ent) const
  {
    
  }
#endif //STUB_ContainsProperty_add

#ifndef STUB_ContainsProperty_copy
//#define STUB_ContainsProperty_copy
  ContainsProperty* ContainsProperty::copy() const
  {
    return nullptr;
  }
#endif //STUB_ContainsProperty_copy


#endif