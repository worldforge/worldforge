#ifndef ERIS_SPAWNPOINT_H_
#define ERIS_SPAWNPOINT_H_

#include <vector>
#include <string>

namespace Eris
{

class CharacterType;

/** Type used to return available character types */
typedef std::vector<CharacterType> CharacterTypeStore;

/**
 * @author Erik Hjortsberg <erik.hjortsberg@gmail.com>
 * @brief Represents a possible spawn point as described by the server.
 * When a new character is created on the server the user can select which
 * "spawn point" the new character should spawn in. Each spawn point in
 * turn will present a list of possible character types to select.
 *
 * It's up to the client to determine how this will be presented. Either the
 * client can first show a list of all available character types, as an
 * aggregate of all spawn points, and then let the user select between
 * different spawn points in the case that the character type is represented in
 * multiple spawn points. Or the client can let the user initially select the
 * spawn point to spawn in, and then present the available character.
 * Or any other method.
 */
class SpawnPoint
{
public:
    /**
     * @brief Ctor.
     * @param name The name of the spawn point.
     * @param availableCharacterTypes A list of available character types.
     * @param description A description of the spawn point.
     */
    SpawnPoint(const std::string& name, const CharacterTypeStore& availableCharacterTypes, const std::string& description);

    /**
     * @brief Dtor.
     */
    virtual ~SpawnPoint();

    /**
     * @brief Gets a list of available character types for this spawn point.
     * @return A list of available character types.
     */
    const CharacterTypeStore& getAvailableCharacterTypes() const;

    /**
     * @brief Gets the name of this spawn point.
     * @returns The name of this spawn point.
     */
    const std::string& getName() const;

    /**
     * @brief Gets a description of this spawn point.
     * @returns A description of this spawn point.
     */
    const std::string& getDescription() const;

private:

    /**
     * @brief The name of this spawn point.
     */
    const std::string m_name;

    /**
     * @brief A list of available character types.
     */
    CharacterTypeStore m_availableCharacterTypes;

    /**
     * @brief A description of this spawn point.
     */
    const std::string m_description;
};

}

#endif /* ERIS_SPAWNPOINT_H_ */
