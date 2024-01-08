#ifndef ERIS_ROUTER_H
#define ERIS_ROUTER_H

#include <Atlas/Objects/ObjectsFwd.h>
#include <Atlas/Objects/Operation.h>

namespace Eris {

/** abstract interface for objects that can route Atlas data. */
struct Router {
	typedef enum {
		IGNORED = 0,
		HANDLED
	} RouterResult;

	virtual ~Router() = default;

	virtual RouterResult handleOperation(const Atlas::Objects::Operation::RootOperation& op) = 0;

};

} // of namespace Eris

#endif
