#ifndef ERIS_TASK_H
#define ERIS_TASK_H

#include <sigc++/trackable.h>
#include <sigc++/signal.h>

#include <map>
#include <string>
#include <vector>

namespace Atlas {
namespace Message {
class Element;

typedef std::map<std::string, Element> MapType;
}
}

namespace WFMath { class TimeDiff; }

namespace Eris {

class Entity;

class View;

struct TaskUsage {
	std::string name;

	bool operator==(const TaskUsage& x) const {
		return name == x.name;
	}

};

class Task : public sigc::trackable {
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
	 * @brief Gets the progress rate.
	 * @return
	 */
	double progressRate() const;

	/**
	 * @brief Returns true if the task has completed.
	 * @returns True if the task has completed.
	 */
	bool isComplete() const;

	const std::vector<TaskUsage>& getUsages() const;

	sigc::signal<void> Completed;

	sigc::signal<void> Progressed;

	sigc::signal<void> ProgressRateChanged;

	sigc::signal<void> UsagesChanged;

private:
	void progressChanged();

	friend class View; // so it can call updateProgress
	friend class Entity; // for constructor and updateFromAtlas

	/**
	Create a new task owned by the specified entity
	*/
	Task(Entity* owner, std::string name);

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

	std::vector<TaskUsage> m_usages;
};

inline const std::string& Task::name() const {
	return m_name;
}

inline double Task::progress() const {
	return m_progress;
}

inline double Task::progressRate() const {
	return m_progressRate;
}


inline const std::vector<TaskUsage>& Task::getUsages() const {
	return m_usages;
}
}

#endif
