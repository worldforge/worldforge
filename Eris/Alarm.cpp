#include <Eris/Alarm.h>
#include <Eris/DeleteLater.h>

namespace Eris
{

Alarm::Alarm(unsigned long msec, const sigc::slot<void>& done)
{
    m_due = WFMath::TimeStamp::now() +  msec;
    AlarmExpired.connect(done);
}

void Alarm::expired()
{
    AlarmExpired();
    deleteLater(this);
}

} // of namespace Eris
