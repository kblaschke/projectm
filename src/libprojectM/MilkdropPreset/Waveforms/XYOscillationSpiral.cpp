#include "Waveforms/XYOscillationSpiral.hpp"

#include "PerFrameContext.hpp"

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

auto XYOscillationSpiral::IsLoop() -> bool
{
    return true;
}

auto XYOscillationSpiral::UsesNormalizedMysteryParam() -> bool
{
    return true;
}

void XYOscillationSpiral::GenerateVertices(const PresetState& presetState,
                                           const PerFrameContext&)
{
    m_samples = Audio::WaveformSamples / 2;

    m_wave1Vertices.resize(m_samples);

    for (uint32_t i = 0; i < m_samples; i++)
    {
        const float radius = (0.53f + 0.43f * m_pcmDataR[i] + m_mysteryWaveParam);
        const double angle = m_pcmDataL[i + 32] * 1.57 + presetState.renderContext.time * 2.3;

        m_wave1Vertices[i] = {radius * static_cast<float>(cos(angle)) * m_aspectY + m_waveX,
                              radius * static_cast<float>(sin(angle)) * m_aspectX + m_waveY};
    }
}

} // namespace Waveforms
} // namespace MilkdropPreset
} // namespace libprojectM
