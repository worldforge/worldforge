#include "Calendar.h"
#include "Avatar.h"
#include "Exceptions.h"
#include "View.h"
#include "Entity.h"
#include "Log.h"
#include "iround.h"

#include <cmath>
#include <utility>

using namespace Atlas::Message;

namespace Eris {

Calendar::Calendar(std::function<std::chrono::milliseconds()> timeProvider) :
		m_timeProvider(std::move(timeProvider)),
		m_daysPerMonth(0),
		m_monthsPerYear(0),
		m_hoursPerDay(0),
		m_minutesPerHour(0),
		m_secondsPerMinute(0) {
}

void Calendar::startObserve(Entity& entity) {
	m_calendarObserver.disconnect();
	m_calendarObserver = entity.observe("calendar", sigc::mem_fun(*this, &Calendar::calendarAttrChanged), true);
}

void Calendar::calendarAttrChanged(const Element& value) {
	//Reset the calendar first; if this is zero the dates will be invalid.
	m_daysPerMonth = 0;

	if (value.isMap()) {
		try {
			initFromCalendarAttr(value.Map());
		} catch (const InvalidAtlas& e) {
			logger->warn("Error when parsing calendar attribute. {}", e.what());
		}
	}
}

void Calendar::initFromCalendarAttr(const MapType& cal) {
	auto it = cal.find("days_per_month");
	if (it == cal.end()) throw InvalidAtlas("malformed calendar data");
	m_daysPerMonth = (int) it->second.asInt();

	it = cal.find("hours_per_day");
	if (it == cal.end()) throw InvalidAtlas("malformed calendar data");
	m_hoursPerDay = (int) it->second.asInt();

	it = cal.find("minutes_per_hour");
	if (it == cal.end()) throw InvalidAtlas("malformed calendar data");
	m_minutesPerHour = (int) it->second.asInt();

	it = cal.find("months_per_year");
	if (it == cal.end()) throw InvalidAtlas("malformed calendar data");
	m_monthsPerYear = (int) it->second.asInt();

	it = cal.find("seconds_per_minute");
	if (it == cal.end()) throw InvalidAtlas("malformed calendar data");
	m_secondsPerMinute = (int) it->second.asInt();

	Updated.emit();
}

DateTime Calendar::now() const {
	DateTime n{};
	// we don't have valid calendar data yet
	if (m_daysPerMonth == 0) return n;

	auto world_time_in_seconds = duration_cast<std::chrono::seconds>(m_timeProvider()).count();

	n.m_seconds = world_time_in_seconds % m_secondsPerMinute;
	world_time_in_seconds /= m_secondsPerMinute;

	n.m_minutes = world_time_in_seconds % m_minutesPerHour;
	world_time_in_seconds /= m_minutesPerHour;

	n.m_hours = world_time_in_seconds % m_hoursPerDay;
	world_time_in_seconds /= m_hoursPerDay;

	n.m_dayOfMonth = world_time_in_seconds % m_daysPerMonth;
	world_time_in_seconds /= m_daysPerMonth;

	n.m_month = world_time_in_seconds % m_monthsPerYear;
	world_time_in_seconds /= m_monthsPerYear;

	n.m_year = static_cast<int>(world_time_in_seconds);

	n.m_valid = true;
	return n;
}


} // of namespace Eris
