#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/Calendar.h>
#include <Eris/Avatar.h>
#include <Eris/Exceptions.h>
#include <Eris/View.h>
#include <Eris/Entity.h>
#include <Eris/Log.h>
#include "iround.h"

#include <cmath>

#include <iostream>

using namespace Atlas::Message;

namespace Eris
{

Calendar::Calendar(Avatar* av) :
    m_avatar(av),
    m_daysPerMonth(0),
    m_monthsPerYear(0),
    m_hoursPerDay(0),
    m_minutesPerHour(0),
    m_secondsPerMinute(0)
{
    assert(av->getView() != NULL);

    av->getView()->TopLevelEntityChanged.connect(
        sigc::mem_fun(this, &Calendar::topLevelEntityChanged));
    // hook up right now if currently valid
    if (av->getView()->getTopLevel()) topLevelEntityChanged();
}

void Calendar::topLevelEntityChanged()
{
    m_calendarObserver.disconnect();
    Entity* tl = m_avatar->getView()->getTopLevel();
    if (!tl || !tl->hasAttr("calendar")) return;

    m_calendarObserver = tl->observe("calendar", sigc::mem_fun(this, &Calendar::calendarAttrChanged));

    calendarAttrChanged(tl->valueOfAttr("calendar"));
}

void Calendar::calendarAttrChanged(const Element& value)
{
    if (!value.isMap()) throw InvalidAtlas("malformed calendar data", value);
    initFromCalendarAttr(value.Map());
}

void Calendar::initFromCalendarAttr(const MapType& cal)
{
    MapType::const_iterator it = cal.find("days_per_month");
    if (it == cal.end()) throw InvalidAtlas("malformed calendar data", cal);
    m_daysPerMonth = it->second.asInt();

    it = cal.find("hours_per_day");
    if (it == cal.end()) throw InvalidAtlas("malformed calendar data", cal);
    m_hoursPerDay = it->second.asInt();

    it = cal.find("minutes_per_hour");
    if (it == cal.end()) throw InvalidAtlas("malformed calendar data", cal);
    m_minutesPerHour = it->second.asInt();

    it = cal.find("months_per_year");
    if (it == cal.end()) throw InvalidAtlas("malformed calendar data", cal);
    m_monthsPerYear = it->second.asInt();

    it = cal.find("seconds_per_minute");
    if (it == cal.end()) throw InvalidAtlas("malformed calendar data", cal);
    m_secondsPerMinute = it->second.asInt();
}

DateTime Calendar::now() const
{
    DateTime n;
    // we don't have valid calendar data yet
    if (m_daysPerMonth == 0) return n;

    long long world_time = L_ROUND(m_avatar->getWorldTime());

    n.m_seconds = world_time % m_secondsPerMinute;
    world_time /= m_secondsPerMinute;

    n.m_minutes = world_time % m_minutesPerHour;
    world_time /= m_minutesPerHour;

    n.m_hours = world_time % m_hoursPerDay;
    world_time /= m_hoursPerDay;

    n.m_dayOfMonth = world_time % m_daysPerMonth;
    world_time /= m_daysPerMonth;

    n.m_month = world_time % m_monthsPerYear;
    world_time /= m_monthsPerYear;

    n.m_year = world_time;

    n.m_valid = true;
    return n;
}

} // of namespace Eris
