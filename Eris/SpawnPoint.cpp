#include "SpawnPoint.h"
#include "CharacterType.h"

namespace Eris
{

SpawnPoint::SpawnPoint(const std::string& name,
        const CharacterTypeStore& availableCharacterTypes,
        const std::string& description) :
    m_name(name), m_availableCharacterTypes(availableCharacterTypes),
            m_description(description)
{
}

SpawnPoint::~SpawnPoint()
{
}

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
