//
// C++ Interface: CaseBase
//
// Description:
//
//
// Author: Erik Ogenvik <erik@ogenvik.org>, (C) 2007
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.//
//
#ifndef EMBEROGRE_MODEL_MAPPING_CASESCASEBASE_H
#define EMBEROGRE_MODEL_MAPPING_CASESCASEBASE_H

#include "../ChangeContext.h"
#include <vector>
#include <memory>

namespace Eris {
class Entity;
}

namespace Ember::EntityMapping {

class ChangeContext;

struct IVisitor;

namespace Actions {
class Action;
}

namespace Matches {
class MatchBase;
}

namespace Cases {

/**
	@brief Base class for all Cases.
	A Case containes zero or many Actions, which will be activated when the Case is activated. A Case also contains zero or many child Matches.
	A Case is activated when it's true and all it's parent cases, all the way up to the root of the EntityMapping, also are true.
	@author Erik Ogenvik <erik@ogenvik.org>
*/
class CaseBase {
public:
	typedef std::vector<std::unique_ptr<Actions::Action>> ActionStore;
	typedef std::vector<std::unique_ptr<Matches::MatchBase>> MatchBaseStore;

	CaseBase();

	virtual ~CaseBase();

	void setParentCase(CaseBase* aCase);

	/**
	 * Returns whether this individual case is true.
	 * @return
	 */
	bool getIsTrue() const;

	/**
	 *    Returns whether this case is active.
	 * @return
	 */
	bool getIsActive() const;

	/**
	 * Returns whether this case is true and all of its parent cases, all the way up to the root, also are true.
	 * @return
	 */
	bool getIsTrueToRoot();

	/**
	Adds an Action to this case.
	*/
	void addAction(std::unique_ptr<Actions::Action> action);

	/**
	Gets a list of all the actions contained by this Case.
	*/
	const ActionStore& getActions();

	/**
	Activates all actions in the Case. Also switches the mActive flag.
	*/
	void activateActions(ChangeContext& context);

	/**
	Deactivates all actions in the Case. Also switches the mActive flag.
	*/
	void deactivateActions(ChangeContext& context);

	/**
	Adds an child Match to this case.
	*/
	void addMatch(std::unique_ptr<Matches::MatchBase> match);

	/**
	Gets a list of all the Matches contained by this Case.
	*/
	const MatchBaseStore& getMatches();

	/**
	Evaluates all changes, and if a change has occurred, adds itself to the supplied ChangeContext instance.
	*/
	void evaluateChanges(ChangeContext& changeContext);

	/**
	Sets the entity that this Case will attach itself to.
	*/
	virtual void setEntity(Eris::Entity* entity);

	/**
	 * @brief Accepts a visitor.
	 * After calling visit() on the visitor, the child actions and matches will be traversed.
	 * @param visitor The visitor instance.
	 */
	void accept(IVisitor& visitor);

	void setState(bool state);

protected:
	ActionStore mActions;

	CaseBase* mParentCase;
	bool mIsTrue, mIsActive;

	MatchBaseStore mMatches;

};

inline void CaseBase::setState(bool state) {
	mIsTrue = state;
}

inline const CaseBase::MatchBaseStore& CaseBase::getMatches() {
	return mMatches;
}

inline const CaseBase::ActionStore& CaseBase::getActions() {
	return mActions;
};

inline void CaseBase::setParentCase(CaseBase* aCase) {
	mParentCase = aCase;
}

inline bool CaseBase::getIsTrue() const {
	return mIsTrue;
}

inline bool CaseBase::getIsActive() const {
	return mIsActive;
}

inline bool CaseBase::getIsTrueToRoot() {
	if (!mParentCase) {
		return mIsTrue;
	} else {
		return mIsTrue && mParentCase->getIsTrueToRoot();
	}
}


}

}



#endif
