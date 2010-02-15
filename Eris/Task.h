#ifndef ERIS_TASK_H
#define ERIS_TASK_H

#include <sigc++/trackable.h>
#include <sigc++/signal.h>

#include <map>
#include <string>

namespace Atlas {
    namespace Message {
        class Element;
        typedef std::map<std::string, Element> MapType;
    }
}

namespace WFMath { class TimeDiff; }

namespace Eris
{

class Entity;
class View;

class Task : public sigc::trackable
{
public:
    virtual ~Task();
    
    /**
     * @brief Gets the name of the task.
     * @return The name of the task.
    */
    const std::string& name() const;
    
    /**
     * @brief Return the current progress of the task. Value will always be in the
     * range [0..1]
     */
    double progress() const;
    
    /**
     * @brief Returns true if the task has completed.
     * @returns True if the task has completed.
     */
    bool isComplete() const;
    
    sigc::signal<void> Completed;
    
    sigc::signal<void> Cancelled;
    
    sigc::signal<void> Progressed;

    sigc::signal<void> ProgressRateChanged;

private:
    void progressChanged();
    
    friend class View; // so it can call updateProgress
    friend class Entity; // for constructor and updateFromAtlas
    
    /**
    Create a new task owned by the specified entity
    */
    Task(Entity* owner, const std::string& nm);
    
    void updateFromAtlas(const Atlas::Message::MapType& d);
    
    /**
    Advance the progress of a constant-rate task
    */
    void updatePredictedProgress(const WFMath::TimeDiff& dt);
        
    const std::string m_name;
    Entity* m_owner;
    double m_progress;
    
    /// progress per second, or 0.0 if progress is non-linear
    double m_progressRate; 
};

inline const std::string& Task::name() const
{
	return m_name;
}

inline double Task::progress() const
{
	return m_progress;
}


}

#endif
