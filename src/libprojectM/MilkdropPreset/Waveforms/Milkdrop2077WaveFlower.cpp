#include "Waveforms/Milkdrop2077WaveFlower.hpp"

#include "PerFrameContext.hpp"

#include <cmath>

namespace libprojectM {
namespace MilkdropPreset {
namespace Waveforms {

bool Milkdrop2077WaveFlower::IsLoop()
{
    return true;
}

void Milkdrop2077WaveFlower::GenerateVertices(const PresetState& presetState,
                                              const PerFrameContext&)
{
    m_samples = Audio::WaveformSamples / 2;

    m_wave1Vertices.resize(m_samples + 1);

    const int sampleOffset = (Audio::WaveformSamples - m_samples) / 2;

    const float invertedSamplesMinusOne = 1.0f / static_cast<float>(m_samples - 1);
    const float tenthSamples = static_cast<float>(m_samples) * 0.1f;

    for (int sample = 0; sample < m_samples; sample++)
    {
        float radius = 0.7f + 0.7f * m_pcmDataR[sample + sampleOffset] + m_mysteryWaveParam;
        const float angle = static_cast<float>(sample) * invertedSamplesMinusOne * 6.28f + presetState.renderContext.time * 0.2f;
        if (static_cast<float>(sample) < static_cast<float>(m_samples) / radius)
        {
            float mix = static_cast<float>(sample) / tenthSamples;
            // Flower
            mix = 0.7f - 0.7f * cosf(mix * 3.1416f);
            // Subtracting the sample offset here instead of adding it, as the original Milkdrop2077 code accessed out-of-range data.
            const float radius2 = 0.7f + 0.7f * m_pcmDataR[sample + m_samples - sampleOffset] + m_mysteryWaveParam;
            radius = radius2 * (1.0f - mix) + radius * mix * .25f;
        }

        m_wave1Vertices[sample] = {
            radius * cosf(angle * 3.1416f) * m_aspectY / 1.5f + m_waveX * cosf(3.1416f),
            radius * sinf(angle - presetState.renderContext.time / 3.0f) * m_aspectX / 1.5f + m_waveY * cosf(3.1416f)};
    }
}

} // namespace Waveforms
} // namespace MilkdropPreset
} // namespace libprojectM
