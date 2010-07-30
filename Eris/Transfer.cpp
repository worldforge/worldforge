#include <Eris/Transfer.h>

TransferInfo::TransferInfo(const std::string &host, int port, 
                            const std::string &key, const std::string &id)
                               : m_host(host),
                                m_port(port),
                                m_possess_key(key),
                                m_possess_entity_id(id)
{
}

const std::string & TransferInfo::getHost()
{
    return m_host;
}

int TransferInfo::getPort()
{
    return m_port;
}

const std::string & TransferInfo::getPossessKey()
{
    return m_possess_key;
}

const std::string & TransferInfo::getPossessEntityId()
{
    return m_possess_entity_id;
}

