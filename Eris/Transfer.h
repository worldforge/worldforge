#ifndef ERIS_TRANSFER_H
#define ERIS_TRANSFER_H

#include <string>

class TransferInfo
{
    std::string m_host;
    int m_port;
    std::string m_possess_key;
    std::string m_possess_entity_id;

    public:

    TransferInfo(const std::string &, int, const std::string &, const std::string &);

    const std::string & getHost();
    int getPort();
    const std::string & getPossessKey();
    const std::string & getPossessEntityId();
};

#endif
