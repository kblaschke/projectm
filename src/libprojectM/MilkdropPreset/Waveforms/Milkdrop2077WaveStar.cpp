#include "Waveforms/Milkdrop2077WaveStar.hpp"

#include "PerFrameContext.hpp"

#include <cmath>

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

bool Milkdrop2077WaveStar::IsLoop()
{
    return true;
}

void Milkdrop2077WaveStar::GenerateVertices(const PresetState& presetState,
                                            const PerFrameContext&)
{
    m_samples = Audio::WaveformSamples / 2;

    m_wave1Vertices.resize(m_samples + 1);

    const uint32_t sampleOffset = (Audio::WaveformSamples - m_samples) / 2;

    const float invertedSamplesMinusOne = 1.0f / static_cast<float>(m_samples - 1);
    const float tenthSamples = static_cast<float>(m_samples) * 0.1f;

    for (uint32_t sample = 0; sample < m_samples; sample++)
    {
        float radius = 0.7f + 0.4f * m_pcmDataR[sample + sampleOffset] + m_mysteryWaveParam;
        const float angle = static_cast<float>(static_cast<double>(sample) * invertedSamplesMinusOne * 6.28 + presetState.renderContext.time * 0.2);
        if (static_cast<float>(sample) < static_cast<float>(m_samples) / radius)
        {
            float mix = static_cast<float>(sample) / tenthSamples;
            mix = 0.5f - 0.5f * cosf(mix * 3.1416f);
            // Subtracting the sample offset here instead of adding it, as the original Milkdrop2077 code accessed out-of-range data.
            const float radius2 = 0.5f + 0.4f * m_pcmDataR[sample + m_samples - sampleOffset] + m_mysteryWaveParam;
            radius = radius2 * (1.0f - mix) + radius * mix;
        }
        m_wave1Vertices[sample] = {radius * cosf(angle) * m_aspectY + m_waveX,
                                   radius * sinf(angle) * m_aspectX + m_waveY};
    }
}

} // namespace Waveforms
} // namespace MilkdropPreset
} // namespace libprojectM
