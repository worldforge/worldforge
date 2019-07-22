#include <utility>

#include <utility>

#include "SpawnPoint.h"
#include "CharacterType.h"

namespace Eris
{

SpawnPoint::SpawnPoint(std::string  name,
        CharacterTypeStore  availableCharacterTypes,
        std::string  description) :
    m_name(std::move(name)), m_availableCharacterTypes(std::move(availableCharacterTypes)),
            m_description(std::move(description))
{
}

SpawnPoint::~SpawnPoint() = default;

const CharacterTypeStore& SpawnPoint::getAvailableCharacterTypes() const
{
    return m_availableCharacterTypes;
}

const std::string& SpawnPoint::getName() const
{
    return m_name;
}

const std::string& SpawnPoint::getDescription() const
{
    return m_description;
}

}
