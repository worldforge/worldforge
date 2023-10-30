#ifndef ERIS_SPAWNPOINT_H_
#define ERIS_SPAWNPOINT_H_

#include <vector>
#include <string>

namespace Eris {

/**
 * @brief An available character type which a client can create a character from on the server.
 * This is presented to the client when first connected to the server and allows a user to select what kind of character to create in the world.
 *
 * @author Erik Hjortsberg <erik.hjortsberg@gmail.com>
 */
struct CharacterType {

	/**
	 * @brief The name of the character type.
	 */
	std::string name;

	/**
	 * @brief A description of the character type.
	 */
	std::string description;
};

struct SpawnProperty {
	enum class Type {
		STRING
	};
	std::string name;
	/**
	 * Show to the user
	 */
	const std::string label;
	std::string description;
	Type type;
	std::vector<Atlas::Message::Element> options;
};
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
struct SpawnPoint {

	std::string id;

	/**
	 * @brief The name of this spawn point.
	 */
	const std::string name;

	/**
	 * @brief A description of this spawn point.
	 */
	const std::string description;

	std::vector<SpawnProperty> properties;

};

}

#endif /* ERIS_SPAWNPOINT_H_ */
