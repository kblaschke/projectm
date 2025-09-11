#include "Triangle.hpp"

#include "PresetState.hpp"

#include <cmath>

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

bool Triangle::IsLoop()
{
    return true;
}

void Triangle::GenerateVertices(const PresetState& presetState, const PerFrameContext&)
{
    m_samples = Audio::WaveformSamples / 2;

    m_wave1Vertices.resize(m_samples + 1);

    const float inverseSamplesMinusOne{1.0f / static_cast<float>(m_samples)};
    const float angleOffset = presetState.renderContext.time * 0.2f;

    for (int sample = 0; sample < m_samples; sample++)
    {
        const float progress = static_cast<float>(sample) * inverseSamplesMinusOne;
        const float phi0 = (std::floor(progress * 3.0f) + .5f) / 3.0f * 6.28f + angleOffset;
        const float angle = (progress * 6.28f) + angleOffset;
        const float edgeDistance = std::cos(angle - phi0);
        const float radius = (0.7f + (edgeDistance * m_pcmDataR[sample]) + m_mysteryWaveParam) / (2.0f * edgeDistance);

        m_wave1Vertices[sample].x = radius * std::cos(angle) * m_aspectY + m_waveX;
        m_wave1Vertices[sample].y = radius * std::sin(angle) * m_aspectX + m_waveY;
    }
}

} // namespace Waveforms
} // namespace MilkdropPreset
} // namespace libprojectM
