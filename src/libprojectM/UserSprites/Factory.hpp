#pragma once

#include "UserSprites/Sprite.hpp"

#include <memory>
#include <string>

namespace libprojectM {
namespace UserSprites {

/**
 * @brief Factory class to create user sprite instances
 * The factory takes the type of a sprite as string and, if known, creates a new sprite of that
 * type. If the type isn't known, no sprite is created.
 */
class Factory
{
public:
    Factory() = delete;

    /**
     * Creates a new sprite of the specified type.
     * @param type The type name of the sprite. Currently, only "milkdrop" is supported.
     * @return A new sprite instance if the type was known, or a nullptr if not or an error occurred.
     */
    static auto CreateSprite(const std::string& type) -> Sprite::Ptr;
};

} // namespace UserSprites
} // namespace libprojectM
