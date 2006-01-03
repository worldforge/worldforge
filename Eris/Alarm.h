#ifndef ERIS_ALARM_H
#define ERIS_ALARM_H

#include <Eris/TimedEventService.h>

#include <sigc++/trackable.h>

namespace Eris
{

class Alarm : public TimedEvent, public sigc::trackable
{
public:
    Alarm(unsigned long, const sigc::slot<void>& done);
    
    virtual void expired();
    
    virtual const WFMath::TimeStamp& due() const
    {
        return m_due;
    }
private:
    sigc::signal<void> AlarmExpired;
    
    WFMath::TimeStamp m_due;
};

} // namespace Eris

#endif // of ERIS_ALARM_H
