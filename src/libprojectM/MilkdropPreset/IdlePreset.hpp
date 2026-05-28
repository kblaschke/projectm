#pragma once

#include <Renderer/RenderContext.hpp>

#include <memory>
#include <string>

namespace libprojectM {

class Preset;

namespace MilkdropPreset {

/**
 * A preset that does not depend on the file system to be loaded. This allows projectM to render
 * something (ie. self indulgent project advertising) even when no valid preset directory is found.
 */
class IdlePresets
{

public:
    /// Allocate a new idle preset instance
    /// \returns a newly allocated auto pointer of an idle preset instance

    /**
     * @brief Allocate a new idle preset instance.
     * @param renderContext The projectM preset render context.
     * @return A newly allocated auto pointer of an idle preset instance
     */
    static std::unique_ptr<Preset> Allocate(const Renderer::RenderContext& renderContext);

private:
    static std::string PresetText();
};

} // namespace MilkdropPreset
} // namespace libprojectM
