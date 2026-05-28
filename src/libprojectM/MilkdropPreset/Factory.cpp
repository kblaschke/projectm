#include "Factory.hpp"

#include "IdlePreset.hpp"
#include "MilkdropPreset.hpp"

namespace libprojectM {
namespace MilkdropPreset {

std::unique_ptr<Preset> Factory::LoadPresetFromFile(const std::string& filename, const Renderer::RenderContext& renderContext)
{
    std::string path;
    auto protocol = Protocol(filename, path);
    if (protocol == "idle")
    {
        return IdlePresets::Allocate(renderContext);
    }

    if (protocol == "" || protocol == "file")
    {
        return std::make_unique<MilkdropPreset>(path, renderContext);
    }

    // ToDO: Throw unsupported protocol exception instead to provide more information.
    return nullptr;
}

std::unique_ptr<Preset> Factory::LoadPresetFromStream(std::istream& data, const Renderer::RenderContext& renderContext)
{
    return std::make_unique<MilkdropPreset>(data, renderContext);
}

} // namespace MilkdropPreset
} // namespace libprojectM
