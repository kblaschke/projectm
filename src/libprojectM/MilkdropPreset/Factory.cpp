#include "Factory.hpp"

#include "IdlePreset.hpp"
#include "MilkdropPreset.hpp"

namespace libprojectM {
namespace MilkdropPreset {

std::unique_ptr<Preset> Factory::LoadPresetFromFile(const std::string& filename)
{
    std::string path;
    const auto protocol = Protocol(filename, path);
    if (protocol == "idle")
    {
        return IdlePresets::allocate();
    }

    if (protocol.empty() || protocol == "file")
    {
        return std::make_unique<MilkdropPreset>(path);
    }

    // ToDO: Throw unsupported protocol exception instead to provide more information.
    return nullptr;
}

std::unique_ptr<Preset> Factory::LoadPresetFromStream(std::istream& data)
{
    return std::make_unique<MilkdropPreset>(data);
}

} // namespace MilkdropPreset
} // namespace libprojectM
