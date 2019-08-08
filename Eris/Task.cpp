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
	auto progress = m_progress;
	auto progressRate = m_progressRate;
    auto it = d.find("progress");
    if (it != d.end() && it->second.Float())
    {
        m_progress = it->second.asFloat();
        if (m_progress != progress) {
			progressChanged();
        }
    }
    
    it = d.find("rate");
    if (it != d.end() && it->second.isFloat())
    {
        m_progressRate = it->second.Float();
        if (m_progressRate != progressRate) {
			ProgressRateChanged.emit();
        }
    }

    std::vector<TaskUsage> usages;
    it = d.find("usages");
	if (it != d.end() && it->second.isList())
	{
		for (auto& entry : it->second.List()) {
			if (entry.isMap()) {
				auto nameI = entry.Map().find("name");
				if (nameI != entry.Map().end() && nameI->second.isString()){
					usages.emplace_back(TaskUsage{nameI->second.String()});
				}
			}
		}
		if (usages != m_usages) {
			m_usages = std::move(usages);
			UsagesChanged.emit();
		}
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
