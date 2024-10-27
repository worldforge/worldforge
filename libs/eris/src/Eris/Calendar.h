#ifndef ERIS_CALENDAR_H
#define ERIS_CALENDAR_H

#include <sigc++/trackable.h>
#include <sigc++/connection.h>
#include <sigc++/signal.h>

#include <map>
#include <string>
#include "Entity.h"


namespace Atlas::Message {
class Element;

typedef std::map<std::string, Element> MapType;
}


namespace Eris {

class Avatar;

class Calendar;

/**
Encapsulate a decoded world time instance
*/
class DateTime {
public:
	DateTime() = default;

	bool valid() const { return m_valid; }

	int year() const { return m_year; }

	int month() const { return m_month; }

	int dayOfMonth() const { return m_dayOfMonth; }

	int seconds() const { return m_seconds; }

	int minutes() const { return m_minutes; }

	int hours() const { return m_hours; }

private:
	friend class Calendar;

	int m_year,
			m_month,
			m_dayOfMonth;

	int m_seconds,
			m_minutes,
			m_hours;

	bool m_valid;
};

class Calendar : public sigc::trackable {
public:
	explicit Calendar(std::function<std::chrono::milliseconds()> timeProvider);

	DateTime now() const;

	int secondsPerMinute() const { return m_secondsPerMinute; }

	int minutesPerHour() const { return m_minutesPerHour; }

	int hoursPerDay() const { return m_hoursPerDay; }

	///Emitted when the calendar is updated.
	mutable sigc::signal<void()> Updated;

	void startObserve(Entity& entity);

protected:
	void calendarAttrChanged(const Atlas::Message::Element& value);

	void initFromCalendarAttr(const Atlas::Message::MapType& cal);

	std::function<std::chrono::milliseconds()> m_timeProvider;

	int m_daysPerMonth,
			m_monthsPerYear,
			m_hoursPerDay,
			m_minutesPerHour,
			m_secondsPerMinute;

	sigc::connection m_calendarObserver;
};

} // of namespace Eris

#endif
