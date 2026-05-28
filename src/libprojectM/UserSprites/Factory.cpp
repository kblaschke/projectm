#include "UserSprites/Factory.hpp"

#include "UserSprites/MilkdropSprite.hpp"

#include <Utils.hpp>

namespace libprojectM {
namespace UserSprites {

auto Factory::CreateSprite(const std::string& type, const GladGLContext& glContext) -> Sprite::Ptr
{
    const auto lowerCaseType = Utils::ToLower(type);

    if (lowerCaseType == "milkdrop")
    {
        return std::make_unique<MilkdropSprite>(glContext);
    }

    return {};
}


} // namespace UserSprites
} // namespace libprojectM
