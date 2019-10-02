#include <utility>

#include <utility>

#include "TransferInfo.h"

namespace Eris {

TransferInfo::TransferInfo(std::string host,
						   int port,
						   std::string key,
						   std::string id)
		: m_host(std::move(host)),
		  m_port(port),
		  m_possess_key(std::move(key)),
		  m_possess_entity_id(std::move(id)) {
}

const std::string& TransferInfo::getHost() const {
	return m_host;
}

int TransferInfo::getPort() const {
	return m_port;
}

const std::string& TransferInfo::getPossessKey() const {
	return m_possess_key;
}

const std::string& TransferInfo::getPossessEntityId() const {
	return m_possess_entity_id;
}

}
