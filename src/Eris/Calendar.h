#ifndef ERIS_CALENDAR_H
#define ERIS_CALENDAR_H

#include <sigc++/trackable.h>
#include <sigc++/connection.h>

#include <map>
#include <string>

namespace Atlas
{
namespace Message
{
class Element;
typedef std::map<std::string, Element> MapType;
}
}

namespace Eris
{

class Avatar;
class Calendar;

/**
Encapsulate a decoded world time instance
*/
class DateTime
{
public:
    DateTime() : m_valid(false) { }

    bool valid() const { return m_valid; }

    unsigned int year() const { return m_year; }
    unsigned int month() const { return m_month; }
    unsigned int dayOfMonth() const { return m_dayOfMonth; }

    unsigned int seconds() const { return m_seconds; }
    unsigned int minutes() const { return m_minutes; }
    unsigned int hours() const { return m_hours; }

private:
    friend class Calendar;

    unsigned int m_year,
        m_month,
        m_dayOfMonth;

    unsigned int m_seconds,
        m_minutes,
        m_hours;

    bool m_valid;
};

class Calendar : public sigc::trackable
{
public:
    Calendar(Avatar*);

    DateTime now() const;

    unsigned int secondsPerMinute() const { return m_secondsPerMinute; }
    unsigned int minutesPerHour() const { return m_minutesPerHour; }
    unsigned int hoursPerDay() const { return m_hoursPerDay; }

    ///Emitted when the calendar is updated.
    sigc::signal<void> Updated;

protected:
    void topLevelEntityChanged();
    void calendarAttrChanged(const Atlas::Message::Element& value);

    void initFromCalendarAttr(const Atlas::Message::MapType& cal);

    Avatar* m_avatar;

    unsigned int m_daysPerMonth,
                 m_monthsPerYear,
                 m_hoursPerDay,
                 m_minutesPerHour,
                 m_secondsPerMinute;

    sigc::connection m_calendarObserver;
};

} // of namespace Eris

#endif
