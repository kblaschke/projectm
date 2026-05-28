#pragma once

#include "PerFrameContext.hpp"
#include "PresetState.hpp"

#include <Renderer/Mesh.hpp>

namespace libprojectM {
namespace MilkdropPreset {

/**
 * @brief Renders a video "echo" (ghost image) effect and gamma adjustments.
 */
class VideoEcho
{
public:
	VideoEcho() = delete;

    /**
     * Constructor.
     * @param presetState The preset state to retrieve the effect data from.
     */
explicit VideoEcho(const PresetState& presetState);

	void Draw();

private:
    void DrawVideoEcho();

    void DrawGammaAdjustment();

    const PresetState& m_presetState; //!< The global preset state.

    std::array<std::array<float, 3>, 4> m_shade; // !< Random, changing color values for the four corners
    Renderer::Mesh m_echoMesh; //!< The video echo/gamma adj mesh
    Renderer::Sampler m_sampler;
};

} // namespace MilkdropPreset
} // namespace libprojectM
