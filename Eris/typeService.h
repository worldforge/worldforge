#ifndef ERIS_TYPE_SERVICE_H
#define ERIS_TYPE_SERVICE_H

#include <sigc++/object.h>
#include <map>
#include <set>

// forward declare some Atlas things
namespace Atlas {
	namespace Message { class Object; }
	namespace Objects { 
		class Root; 
		namespace Operation {
			class Get;
			class Error;
		}
	}
}

namespace Eris {

class Connection;
class TypeInfo;

typedef TypeInfo* TypeInfoPtr;
typedef std::set<TypeInfoPtr> TypeInfoSet;
	
class TypeService : virtual public SigC::Object
{
 public:
	TypeService(Connection *conn);

	void init();

	/// load the core Atlas::Objects specification from the named file
	void readAtlasSpec(const std::string &specfile);
	/** find the TypeInfo for the named type; this may involve a search, or a map lookup. 
	 The returned TypeInfo	
	node may not be bound, and the caller should verify this before using the type. */
	TypeInfoPtr getTypeByName(const std::string &tynm);
	
	/** retrive the TypeInfo for an object; this should be faster (hoepfully constant time) since it
	can take advantage of integer typeids */
	TypeInfoPtr getTypeForAtlas(const Atlas::Message::Object &msg);
	TypeInfoPtr getTypeForAtlas(const Atlas::Objects::Root &obj);
	
	/** Lookup the requested type, by name, and return NULL if it's unknown. */
	TypeInfoPtr findTypeByName(const std::string &tynm);
	
	/** emitted when a new type is available and bound to it's parents */
	SigC::Signal1<void, TypeInfo*> BoundType;
	
	void listUnbound();

 protected:
		 
	friend class TypeInfo;
		
	TypeInfoSet extractDependantsForType(const std::string &typeName);
 
	void markTypeDependantOnType(TypeInfoPtr dep, TypeInfoPtr ancestor);
	
 private:

	void sendInfoRequest(const std::string &id);
	void recvInfoOp(const Atlas::Objects::Root &atype);
	
	void recvTypeError(const Atlas::Objects::Operation::Error &error,
		const Atlas::Objects::Operation::Get &get);
	
	/// build a TypeInfo object if necessary, and add it to the database
	void registerLocalType(const Atlas::Objects::Root &def);
	
	typedef std::map<std::string, TypeInfoPtr> TypeInfoMap;
	/** The easy bit : a simple map from 'string-id' (e.g 'look' or 'farmer')
	to the corresponding TypeInfo instance. This could be a hash_map in the
	future, if efficeny consdierations indicate it would be worthwhile */
	TypeInfoMap globalTypeMap;

	typedef std::map<std::string, TypeInfoSet> TypeDepMap;

	/** This is a dynamic structure indicating which Types are blocked
	awaiting INFOs from other ops. For each blocked INFO, the first item
	is the <i>blocking</i> type (e.g. 'tradesman') and the second item
	the blocked TypeInfo, (e.g. 'blacksmith')*/
	TypeDepMap _dependancyMap;

	Connection* _conn;
	bool _inited;
};

} // of namespace Eris

#endif // of ERIS_TYPE_SERVICE_H
