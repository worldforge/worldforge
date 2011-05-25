#ifndef ERIS_CHARACTERTYPE_H_
#define ERIS_CHARACTERTYPE_H_

#include <string>

namespace Eris
{

/**
 * @brief An available character type which a client can create a character from on the server.
 * This is presented to the client when first connected to the server and allows a user to select what kind of character to create in the world.
 *
 * @author Erik Hjortsberg <erik.hjortsberg@gmail.com>
 */
class CharacterType
{
public:
    /**
     * @brief Ctor.
     * @param name The name of the character type.
     * @param description A description to show the user.
     */
    CharacterType(const std::string& name, const std::string& description);

    /**
     * @brief Accessor for the name of the character type.
     * @returns The name of the character type.
     */
    const std::string& getName() const;

    /**
     * @brief Accessor for the description of the character type.
     * @returns A description of the character type.
     */
    const std::string& getDescription() const;

private:

    /**
     * @brief The name of the character type.
     */
    std::string m_name;

    /**
     * @brief A description of the character type.
     */
    std::string m_description;
};

}

#endif /* ERIS_CHARACTERTYPE_H_ */
