#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <Eris/typeService.h>

namespace Eris
{
	
TypeInfoSet TypeService::extractDependantsForType(const std::string &typeName)
{
	TypeInfoSet result;
	
	TypeDepMap::iterator D = _dependancyMap.find(typeName);
	if (D != _dependancyMap.end()) {
		result = D->second; // take a copy
		_dependancyMap.erase(D);
	}
	
	return result;
}		
	
void TypeService::markTypeDependantOnType(TypeInfoPtr dep, TypeInfoPtr ancestor)
{
	TypeDepMap::iterator D = _dependancyMap.find(ancestor->getName());
	if (D == _DependancyMap.end()) {
		// insert an empty dependant set into the map
		D = _dependancyMap.insert(D, TypeDepMap::value_type(ancestor->getName(), TypeInfoSet()));
	}
			
	if (D->second.count(dep)) {
		Eris::log(LOG_WARNING, "marking type %s as dependent on type %s, but it was already marked as such",
					dep->getName().c_str(), 
					ancestor->getName().c_str());
	} else {
		Eris::log(LOG_DEBUG, "marking type %s as dependent on type %s",
					dep->getName().c_str(), 
					ancestor->getName().c_str());
	}
			
			// becuase we're doing a set<> insert, there is no problem with duplicates!
			Ddep->second.insert(this);
}

} // of namespace Eris
