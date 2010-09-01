// Eris Online RPG Protocol Library
// Copyright (C) 2007 Alistair Riddoch
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

// $Id$

#include <Eris/Entity.h>

#include <Eris/Alarm.h>
#include <Eris/Exceptions.h>
#include <Eris/Log.h>
#include <Eris/Task.h>
#include <Eris/TypeInfo.h>

class TestErisEntity : public Eris::Entity
{
  public:
    TestErisEntity(const std::string & id, Eris::TypeInfo * tp) : Eris::Entity(id, tp) { }

    virtual Eris::TypeService* getTypeService() const { return 0; }
    virtual void removeFromMovementPrediction() { }
    virtual void addToMovementPredition() { }
    virtual Eris::Entity* getEntity(const std::string&) { return 0; }

};

int main()
{
    {
        Eris::Entity * e = new TestErisEntity("1", 0);
        e->shutdown();
        delete e;
    }


    return 0;
}

// stubs

namespace Eris {

Alarm::Alarm(unsigned long msec, const sigc::slot<void>& done)
{
}

void Alarm::expired()
{
}

const Atlas::Message::Element* TypeInfo::getAttribute(const std::string& attributeName) const
{
    return 0;
}

TypeInfoPtr TypeService::getTypeByName(const std::string &id)
{
    return 0;
}

Task::Task(Entity* owner, const std::string& nm) :
    m_name(nm),
    m_owner(owner),
    m_progress(0.0),
    m_progressRate(-1.0)
{
}

Task::~Task()
{
}

void Task::updateFromAtlas(const Atlas::Message::MapType & d)
{
}

BaseException::~BaseException() throw()
{
}

InvalidOperation::~InvalidOperation() throw()
{
}

void doLog(LogLevel lvl, const std::string& msg)
{
}

void mergeOrCopyElement(const Atlas::Message::Element& src, Atlas::Message::Element& dst)
{
}

}
