#ifndef ERIS_TRANSFERINFO_H
#define ERIS_TRANSFERINFO_H

#include <string>

namespace Eris
{

/** A TransferInfo object represents the encapsulation of various data required
to successfully transfer a character from one server to another. The data includes
the new host and port of the transfer destination as well as fields to authenticate
the client as the owner of the transferred character. A TransferInfo object is 
normally created during the servers reply to a transfer request initiated by the
client (eg. a teleport request)
*/
class TransferInfo
{
public:
    /**
     * @brief Ctor
     * @param host The transfer destination servers hostname
     * @param port The transfer destination servers port
     * @param key Randomized key used to claim ownership over transferred character
     * @param id Entity ID of transferred character on destination server
     */
    TransferInfo(const std::string &host, int port, const std::string &key, const std::string &id);

    /**
     * @brief Get the hostname of the transfer destination server
     * @return The hostname of the transfer destination
     */
    const std::string & getHost() const;
    
    /**
     * @brief Get the connection port of the transfer destination server
     * @return The connection port of the transfer destination server
     */
    int getPort() const;

    /**
     * @brief Get the one-time generated possess key (used for claiming ownership)
     * @return The possess key to claim ownership of a transferred character
     */
    const std::string & getPossessKey() const;

    /**
     * @brief Get the entity ID of the transferred character on the destination server
     * @return The entity ID of the transferred character on the destination server
     */
    const std::string & getPossessEntityId() const;

private:
    std::string m_host;     ///< The transfer destination servers hostname
    int m_port;     ///< The transfer destination servers port
    std::string m_possess_key;  ///< A randomized one-time key to claim ownership over a transferred character
    std::string m_possess_entity_id;    ///< The entity ID of the trasnferred character on the destination server

};

}

#endif
