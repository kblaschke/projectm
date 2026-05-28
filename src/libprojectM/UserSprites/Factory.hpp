#pragma once

#include "UserSprites/Sprite.hpp"

#include <memory>
#include <string>

namespace libprojectM {
namespace UserSprites {

class Factory
{
public:
    Factory() = delete;

    /**
     * Creates a new sprite with the given type name.
     * @param type Thy sprite type to create, e.g. "milkdrop". Case-insensitive.
     * @param glContext The OpenGL context to use.
     * @return A sprite instance of the given type, or nullptr if the type wasn't recognized.
     */
    static auto CreateSprite(const std::string& type, const GladGLContext& glContext) -> Sprite::Ptr;
};

} // namespace UserSprites
} // namespace libprojectM
