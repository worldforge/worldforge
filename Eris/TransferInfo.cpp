#include "TransferInfo.h"

namespace Eris
{

TransferInfo::TransferInfo(const std::string &host, int port, 
                            const std::string &key, const std::string &id)
                               : m_host(host),
                                m_port(port),
                                m_possess_key(key),
                                m_possess_entity_id(id)
{
}

const std::string & TransferInfo::getHost() const
{
    return m_host;
}

int TransferInfo::getPort() const
{
    return m_port;
}

const std::string & TransferInfo::getPossessKey() const
{
    return m_possess_key;
}

const std::string & TransferInfo::getPossessEntityId() const
{
    return m_possess_entity_id;
}

}
