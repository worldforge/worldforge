// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright 2000-2001 Aloril.

#include <Atlas/Objects/loadDefaults.h>

#include <Atlas/Objects/Dispatcher.h>
#include <Atlas/Objects/objectFactory.h>

#include <Atlas/Codecs/XML.h>

#include <fstream>
#include <string>
#include <map>
#include <set>

using Atlas::Message::Element;

namespace Atlas { namespace Objects {

typedef std::map<std::string, Element> MessageElementMap;

class LoadDefaultsDecoder : public Dispatcher
{
  public:
    LoadDefaultsDecoder(const std::string& filename);
    Element getMessageElement(const std::string& id);
  protected:
    virtual void messageArrived(const Element::MapType&);
  private:
    void setAttributes(Root &obj, //Root &obj_inst, 
                       const Element& mobj, 
                       std::set<std::string> used_attributes);
    void fillDefaults();
    MessageElementMap m_objects;
};

LoadDefaultsDecoder::LoadDefaultsDecoder(const std::string& filename)
{
    std::ifstream stream;
    stream.open(filename.c_str());
    if (!stream) {
        throw DefaultLoadingException("Failed to open file " + filename);
    }
  
  //replace following code with:
  //getCodecByName(const std::string& name, Stream& stream, Bridge* bridge)

  //typedef list<Atlas::Factory<Atlas::Codec<std::iostream> >*> FactoryCodecs;
  //FactoryCodecs *myCodecs = Factory<Codec<std::iostream> >::factories();
  //FactoryCodecs::iterator i;
  //Atlas::Codec<std::iostream> *codec = NULL;
  //for (i = myCodecs->begin(); i != myCodecs->end(); ++i)
    //if((*i)->getName() == "XML")
      //codec = (*i)->New(Codec<std::iostream>::Parameters((iostream&)stream, this));
  //end of replace
    Atlas::Codec *codec = new Atlas::Codecs::XML((std::iostream&)stream, this);

    if (!codec) {
        throw DefaultLoadingException("XML codec not found");
    }

    while(stream) {
        codec->poll();
    }

    delete codec;
  
    Element::MapType anonymous_obj;
    m_objects["anonymous"] = anonymous_obj;
    fillDefaults();
}

Element LoadDefaultsDecoder::getMessageElement(const std::string& id)
{
    MessageElementMap::const_iterator I = m_objects.find(id);
    if (I == m_objects.end()) {
        throw DefaultLoadingException(id + " not found in XML file");
    } else {
        return (*I).second;
    }
}

void LoadDefaultsDecoder::messageArrived(const Element::MapType& o)
{
    MessageElementMap::const_iterator I = o.find("id");
    if (I == o.end()) {
        unknownMessageArrived(o);
        return;
    }
    
    std::string id(I->second.asString());
    m_objects[id] = o;
}

void LoadDefaultsDecoder::setAttributes(Root &obj, //Root &obj_inst, 
                                        const Element& mobj, 
                                        std::set<std::string> used_attributes)
{
    Element::MapType::const_iterator I;
    for (I = mobj.asMap().begin(); I != mobj.asMap().end(); I++) {
        std::set<std::string>::const_iterator attr_found = 
                               used_attributes.find(I->first);
        if (attr_found == used_attributes.end()) {
            //cout<<"    -->"<<I->first<<endl;
            obj->setAttr(I->first, I->second);
            //obj_inst->setAttr(I->first, I->second);
        }
        used_attributes.insert(I->first);
    }
    I = mobj.asMap().find("parents");
    if (I != mobj.asMap().end()) {
        for (Element::ListType::const_iterator J = I->second.asList().begin();
             J != I->second.asList().end(); J++) {
            //cout<<"  >"<<J->asString()<<endl;
            Element parent_mobj = getMessageElement(J->asString());
            setAttributes(obj, /*obj_inst,*/ parent_mobj, used_attributes);
        }
    }
}

void LoadDefaultsDecoder::fillDefaults()
{
    std::list<std::string> keys = objectFactory.getKeys();
    for(std::list<std::string>::const_iterator I = keys.begin();
        I != keys.end();
        I++) {
        //cout<<(*I)<<endl;
        //get atlas.xml object
        Element mobj = getMessageElement(*I);
        //get class instances
        Root obj = objectFactory.createObject(*I).getDefaultObject();
        //Root obj_inst = objectInstanceFactory.createObject(*I).getDefaultObject();
        //add attributes recursively
        std::set<std::string> used_attributes;
        setAttributes(obj, /*obj_inst,*/ mobj, used_attributes);

        //add object definition
        Root obj_def = objectFactory.createObject(*I);
        obj_def->setObjtype(obj->getObjtype());
        Element::MapType::const_iterator J;
        for (J = mobj.asMap().begin(); J != mobj.asMap().end(); J++) {
            obj_def->setAttr(J->first, J->second);
        }
        objectDefinitions[obj_def->getId()] = obj_def;

        //modify attributes in instance that differ from definitions
        std::list<std::string> parents;
        parents.push_back(std::string(*I));
        obj/*_inst*/->setParents(parents);
        obj/*_inst*/->setId("");
        if(obj/*_inst*/->getObjtype()=="op_definition")
            obj/*_inst*/->setObjtype("op");
        else
            obj/*_inst*/->setObjtype("obj");
    }
}

void loadDefaults(const std::string& filename)
{
     LoadDefaultsDecoder load_defaults(filename);
     Entity::Anonymous e;
     e = e->getDefaultObject();
     std::vector<double> coords(3, 0.0);
     e->setPos(coords);
     e->setVelocity(coords);
     std::list<std::string> parents;
     e->setParents(parents);
}

} } // namespace Atlas::Objects
