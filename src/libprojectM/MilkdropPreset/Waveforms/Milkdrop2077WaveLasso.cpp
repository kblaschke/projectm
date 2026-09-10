#include "Waveforms/Milkdrop2077WaveLasso.hpp"

#include "PerFrameContext.hpp"

#include <cmath>

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

void Milkdrop2077WaveLasso::GenerateVertices(const PresetState& presetState,
                                             const PerFrameContext&)
{
    m_samples = Audio::WaveformSamples / 2;

    m_wave1Vertices.resize(m_samples);

    for (uint32_t sample = 0; sample < m_samples; sample++)
    {
        const float angle = m_pcmDataL[sample + 32] * 1.57f + presetState.renderContext.time * 2.0f;

        m_wave1Vertices[sample] = {
            static_cast<float>(cos(presetState.renderContext.time)) / 2.0f + cosf(angle * 2.0f + static_cast<float>(tan(presetState.renderContext.time / angle))),
            static_cast<float>(sin(presetState.renderContext.time)) * 2.0f * sinf(angle * 3.14f) * m_aspectX / 2.8f + m_waveY};
    }
}

} // namespace Waveforms
} // namespace MilkdropPreset
} // namespace libprojectM
