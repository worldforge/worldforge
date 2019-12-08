#ifndef ERIS_TYPE_SERVICE_H
#define ERIS_TYPE_SERVICE_H

#include <Atlas/Objects/ObjectsFwd.h>

#include <sigc++/trackable.h>
#include <sigc++/signal.h>

#include <unordered_map>
#include <set>
#include <string>
#include <memory>

namespace Eris {

class Connection;
class TypeInfo;


/**
 * A service class querying and caching types.
 **/
class TypeService : virtual public sigc::trackable
{
public:
    explicit TypeService(Connection &con);
    virtual ~TypeService();

    void init();

    /** find the TypeInfo for the named type; this may involve a search, or a map lookup.
     The returned TypeInfo node may not be bound, and the caller should verify this
     before using the type. */
    TypeInfo* getTypeByName(const std::string &tynm);

    /** retrive the TypeInfo for an object; this should be faster (hopefully constant time) since it
    can take advantage of integer typeids */
    TypeInfo* getTypeForAtlas(const Atlas::Objects::Root &obj);

    /** Lookup the requested type, by name, and return nullptr if it's unknown. */
    TypeInfo* findTypeByName(const std::string &tynm);

    /** emitted when a new type is available and bound to it's parents */
    sigc::signal<void, TypeInfo*> BoundType;

    /** emitted when a type is confirmed as being undefined */
    sigc::signal<void, TypeInfo*> BadType;

    void handleOperation(const Atlas::Objects::Operation::RootOperation&);

    /** request the information about a type from the server.
    @param id The ID of the type to lookup
    */
    void sendRequest(const std::string& id);

    /**
     * @brief Set another provider of type data than the connection.
     *
     * This should be set to the external mind once an entity has been possessed, since
     * the external mind has access to more type data (for example the type of the entity itself).
     *
     * @param id
     */
    void setTypeProviderId(std::string id);

protected:

    void recvTypeInfo(const Atlas::Objects::Root &atype);
    void recvError(const Atlas::Objects::Operation::Get& get);
    void recvTypeUpdate(const Atlas::Objects::Root &atype);

    TypeInfo* defineBuiltin(const std::string& name, TypeInfo* parent);

    /** The easy bit : a simple map from 'string-id' (e.g 'look' or 'farmer')
    to the corresponding TypeInfo instance. This could be a hash_map in the
    future, if efficiency considerations indicate it would be worthwhile */
	std::unordered_map<std::string, std::unique_ptr<TypeInfo>> m_types;

    Connection& m_con;
    bool m_inited;

    /**
     * An optional type provider, to which requests for types are sent.
     */
    std::string m_type_provider_id;
};

} // of namespace Eris

#endif // of ERIS_TYPE_SERVICE_H
