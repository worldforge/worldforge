// Eris Online RPG Protocol Library
// Copyright (C) 2012 Erik Ogenvik
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#include <sigc++/slot.h>
#include <Atlas/Message/Element.h>

class ElementParam
{
public:
    Atlas::Message::Element defaultElement;

    ElementParam(Atlas::Message::Element defaultElement) :
            defaultElement(defaultElement)
    {
    }

    Atlas::Message::Element::Type getType() const
    {
        return defaultElement.getType();
    }
};

/**
 * @brief Exercises a method which parses atlas data by sending it bogus data.
 *
 * Any method which parses Atlas data received over the network must be able
 * to properly handle incorrect data without throwing an exception or crashing.
 * This class tests this by trying to send incorrect to the method.
 */
template<typename T>
class ElementExerciser
{
    typedef T EntityType;
private:

    std::map<std::string, ElementParam> mElementParams;

    sigc::slot<void, const EntityType&> mExerciseSlot;

    void fillParam(EntityType& entity, const std::string& paramName,
            Atlas::Message::Element::Type paramType)
    {
        switch (paramType) {
        case Atlas::Message::Element::TYPE_FLOAT:
            entity->setAttr(paramName, Atlas::Message::FloatType());
            break;
        case Atlas::Message::Element::TYPE_INT:
            entity->setAttr(paramName, Atlas::Message::IntType());
            break;
        case Atlas::Message::Element::TYPE_LIST:
            entity->setAttr(paramName, Atlas::Message::ListType());
            break;
        case Atlas::Message::Element::TYPE_MAP:
            entity->setAttr(paramName, Atlas::Message::MapType());
            break;
        case Atlas::Message::Element::TYPE_PTR:
            entity->setAttr(paramName, Atlas::Message::PtrType());
            break;
        case Atlas::Message::Element::TYPE_STRING:
            entity->setAttr(paramName, Atlas::Message::StringType());
            break;
        case Atlas::Message::Element::TYPE_NONE:
            entity->setAttr(paramName, Atlas::Message::Element());
            break;
        }

    }

    void fillCorrectParam(EntityType& entity, const std::string& paramName)
    {
        const ElementParam& param = mElementParams.find(paramName)->second;
        fillParam(entity, paramName, param.getType());
    }

    void fillAllCorrectParams(EntityType& entity)
    {
        for (std::map<std::string, ElementParam>::const_iterator I =
                mElementParams.begin(); I != mElementParams.end(); ++I) {
            fillParam(entity, I->first, I->second.getType());
        }

    }

public:

    ElementExerciser(sigc::slot<void, const EntityType&> exerciseSlot) :
            mExerciseSlot(exerciseSlot)
    {
    }

    void addParam(const std::string& paramName, ElementParam param)
    {
        mElementParams.insert(std::make_pair(paramName, param));
    }

    void exercise()
    {
        static Atlas::Message::Element::Type types[] = {
                Atlas::Message::Element::TYPE_FLOAT,
                Atlas::Message::Element::TYPE_INT,
                Atlas::Message::Element::TYPE_LIST,
                Atlas::Message::Element::TYPE_MAP,
                Atlas::Message::Element::TYPE_NONE,
                Atlas::Message::Element::TYPE_PTR,
                Atlas::Message::Element::TYPE_STRING };

        mExerciseSlot(EntityType());

        //First test by submitting correct data.
        {
            EntityType entity;
            fillAllCorrectParams(entity);
            mExerciseSlot(entity);
        }

        //Then test by submitting an emtpy element.
        mExerciseSlot(EntityType());

        //Then try to send incorrect data. We'll go through all params, and for
        //each params we'll try to send an instance of each message type.
        //If the method is correctly written it will handle these without
        //throwing an exception or crashing.
        for (std::map<std::string, ElementParam>::const_iterator I =
                mElementParams.begin(); I != mElementParams.end(); ++I) {
            for (size_t i = 0; i < (sizeof(types) / sizeof(*types)); ++i) {
                Atlas::Message::Element::Type type = types[i];
                EntityType entity;
                fillAllCorrectParams(entity);
                fillParam(entity, I->first, type);
                mExerciseSlot(entity);
            }

        }

    }

};
