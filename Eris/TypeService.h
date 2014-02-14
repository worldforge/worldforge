#ifndef ERIS_TYPE_SERVICE_H
#define ERIS_TYPE_SERVICE_H

#include <Atlas/Objects/ObjectsFwd.h>

#include <sigc++/trackable.h>
#include <sigc++/signal.h>

#include <unordered_map>
#include <set>
#include <string>

namespace Eris {

class Connection;
class TypeInfo;

typedef TypeInfo* TypeInfoPtr;
typedef std::set<TypeInfoPtr> TypeInfoSet;

/**
 * A service class querying and caching types.
 **/
class TypeService : virtual public sigc::trackable
{
public:
    TypeService(Connection *con);
    virtual ~TypeService();

    void init();

    /** find the TypeInfo for the named type; this may involve a search, or a map lookup.
     The returned TypeInfo node may not be bound, and the caller should verify this
     before using the type. */
    TypeInfoPtr getTypeByName(const std::string &tynm);

    /** retrive the TypeInfo for an object; this should be faster (hopefully constant time) since it
    can take advantage of integer typeids */
    TypeInfoPtr getTypeForAtlas(const Atlas::Objects::Root &obj);

    /** Lookup the requested type, by name, and return NULL if it's unknown. */
    TypeInfoPtr findTypeByName(const std::string &tynm);

    /** emitted when a new type is available and bound to it's parents */
    sigc::signal<void, TypeInfoPtr> BoundType;

    /** emitted when a type is confirmed as being undefined */
    sigc::signal<void, TypeInfoPtr> BadType;

    void listUnbound();

    void handleOperation(const Atlas::Objects::Operation::RootOperation&);

 protected:
    /** request the information about a type from the server.
    @param id The ID of the type to lookup
    */
    void sendRequest(const std::string& id);
    void recvTypeInfo(const Atlas::Objects::Root &atype);
    void recvError(const Atlas::Objects::Operation::Get& get);

    TypeInfoPtr defineBuiltin(const std::string& name, TypeInfoPtr parent);

    typedef std::unordered_map<std::string, TypeInfoPtr> TypeInfoMap;
    /** The easy bit : a simple map from 'string-id' (e.g 'look' or 'farmer')
    to the corresponding TypeInfo instance. This could be a hash_map in the
    future, if efficeny consdierations indicate it would be worthwhile */
    TypeInfoMap m_types;

    Connection* m_con;
    bool m_inited;
};

} // of namespace Eris

#endif // of ERIS_TYPE_SERVICE_H
