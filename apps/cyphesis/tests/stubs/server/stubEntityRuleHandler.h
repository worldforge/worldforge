// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubEntityRuleHandler_custom.h file.

#ifndef STUB_SERVER_ENTITYRULEHANDLER_H
#define STUB_SERVER_ENTITYRULEHANDLER_H

#include "server/EntityRuleHandler.h"
#include "stubEntityRuleHandler_custom.h"

#ifndef STUB_EntityRuleHandler_populateEntityFactory
//#define STUB_EntityRuleHandler_populateEntityFactory
  int EntityRuleHandler::populateEntityFactory(const std::string& class_name, EntityFactoryBase* factory, const Atlas::Message::MapType& class_desc, std::string& dependent, std::string& reason)
  {
    return 0;
  }
#endif //STUB_EntityRuleHandler_populateEntityFactory

#ifndef STUB_EntityRuleHandler_installEntityClass
//#define STUB_EntityRuleHandler_installEntityClass
  int EntityRuleHandler::installEntityClass(const std::string& class_name, const std::string& parent, const Atlas::Objects::Root& class_desc, std::string& dependent, std::string& reason, std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes)
  {
    return 0;
  }
#endif //STUB_EntityRuleHandler_installEntityClass

#ifndef STUB_EntityRuleHandler_installEntityClass
//#define STUB_EntityRuleHandler_installEntityClass
  int EntityRuleHandler::installEntityClass(const std::string& class_name, const std::string& parent, const Atlas::Objects::Root& class_desc, std::string& dependent, std::string& reason, std::unique_ptr<EntityFactoryBase> factory, std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes)
  {
    return 0;
  }
#endif //STUB_EntityRuleHandler_installEntityClass

#ifndef STUB_EntityRuleHandler_modifyEntityClass
//#define STUB_EntityRuleHandler_modifyEntityClass
  int EntityRuleHandler::modifyEntityClass(const std::string& class_name, const Atlas::Objects::Root& class_desc, std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes)
  {
    return 0;
  }
#endif //STUB_EntityRuleHandler_modifyEntityClass

#ifndef STUB_EntityRuleHandler_EntityRuleHandler
//#define STUB_EntityRuleHandler_EntityRuleHandler
   EntityRuleHandler::EntityRuleHandler(EntityBuilder& eb, const PropertyManager& propertyManager)
    : RuleHandler(eb, propertyManager)
  {
    
  }
#endif //STUB_EntityRuleHandler_EntityRuleHandler

#ifndef STUB_EntityRuleHandler_check
//#define STUB_EntityRuleHandler_check
  int EntityRuleHandler::check(const Atlas::Objects::Root& desc)
  {
    return 0;
  }
#endif //STUB_EntityRuleHandler_check

#ifndef STUB_EntityRuleHandler_install
//#define STUB_EntityRuleHandler_install
  int EntityRuleHandler::install(const std::string& class_name, const std::string& parent, const Atlas::Objects::Root& desc, std::string& dependent, std::string& reason, std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes)
  {
    return 0;
  }
#endif //STUB_EntityRuleHandler_install

#ifndef STUB_EntityRuleHandler_update
//#define STUB_EntityRuleHandler_update
  int EntityRuleHandler::update(const std::string& name, const Atlas::Objects::Root& desc, std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes)
  {
    return 0;
  }
#endif //STUB_EntityRuleHandler_update


#endif