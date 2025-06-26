#include "SpectrumBars.hpp"


namespace libprojectM {
namespace MilkdropPreset {

SpectrumBars::SpectrumBars()
{
}

SpectrumBars::~SpectrumBars()
{
}

void SpectrumBars::InitVertexAttrib()
{
}

void SpectrumBars::Initialize(::libprojectM::PresetFileParser& parsedFile)
{
    int32_t barsMode = parsedFile.GetInt("bars_mode", 0);
    if (barsMode <= 0 || barsMode >= Mode::Count)
    {
        return;
    }

    m_mode = static_cast<Mode>(barsMode);

}

void SpectrumBars::Draw()
{
}

} // MilkdropPreset
} // libprojectM
