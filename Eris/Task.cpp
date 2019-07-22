#include <utility>

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "Task.h"
#include "View.h"
#include "Entity.h"

#include <Atlas/Message/Element.h>

#include <wfmath/timestamp.h>

typedef Atlas::Message::MapType AtlasMapType;

namespace Eris
{

Task::Task(Entity* owner, std::string name) :
    m_name(std::move(name)),
    m_owner(owner),
    m_progress(0.0),
    m_progressRate(-1.0)
{
    
}

Task::~Task()
{
    m_progressRate = -1.0;
    // force it to be un-registered
    ProgressRateChanged.emit();
}

bool Task::isComplete() const
{
    return (m_progress >= 1.0);
}

void Task::updateFromAtlas(const AtlasMapType& d)
{
    auto it = d.find("progress");
    if (it != d.end())
    {
        m_progress = it->second.asFloat();
        progressChanged();
    }
    
    it = d.find("rate");
    if (it != d.end())
    {
        m_progressRate = it->second.asFloat();
        ProgressRateChanged.emit();
    }
}

void Task::progressChanged()
{
    Progressed.emit();
    if (isComplete())
    {
        Completed.emit();
        
        // remove from progression updating
        m_progressRate = -1;
        ProgressRateChanged.emit();
    }
}

void Task::updatePredictedProgress(const WFMath::TimeDiff& dt)
{
    if (isComplete()) return;
    
    m_progress += m_progressRate * (dt.milliseconds() / 1000.0);
    m_progress = std::min(m_progress, 1.0);
    
    Progressed.emit();
    // note we will never signal completion here, but instead we wait for
    // the server to notify us.
}

}
