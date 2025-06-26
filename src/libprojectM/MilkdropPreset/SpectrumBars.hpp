#pragma once

#include "Constants.hpp"
#include "PresetState.hpp"

#include <PresetFileParser.hpp>

#include <Renderer/RenderItem.hpp>

#include <optional>

namespace libprojectM {
namespace MilkdropPreset {

/**
 * @brief Renders the Poweramp spectrum bars effect.
 * @see https://github.com/SpasilliumNexus/poweramp-visualizer-presets
 */
class SpectrumBars : public Renderer::RenderItem
{
public:
    SpectrumBars();

    ~SpectrumBars() override;

    void InitVertexAttrib() override;

    /**
     * @brief Loads the initial values and code from the preset file.
     * @param parsedFile The file parser with the preset data.
     */
    void Initialize(::libprojectM::PresetFileParser& parsedFile);

    /**
     * @brief Renders the spectrum bar effect.
     */
    void Draw();

private:
    /**
     * Rendering mode
     */
    enum Mode
    {
        Disabled = 0,             //!< Disabled, no spectrum bars
        LongBars = 1,             //!< Long bars
        Rectangles = 2,           //!< Rectangles
        LongBarsCentered = 3,     //!< Long bars, centered vertically
        RectanglesCentered = 4,   //!< Rectangles, centered vertically
        TopAlignedBars = 5,       //!< Bars aligned to the top of the screen
        LongBarsReflectedX = 6,   //!< Long bars reflected to left/right
        RectanglesReflectedX = 7, //!< Rectangles reflected to left/right
        Count = 8
    };

    Mode m_mode{Disabled}; //!< Render mode. Parsed from bars_mode.

    int32_t m_numX{0}; //!< Number of bars in X axis, max=128 for long bars; 32 for rectangles. Parsed from bars_num_x.
    int32_t m_numY{0}; //!< Number of rectangles in Y axis, max=24. Parsed from bars_num_y.

    float m_spacingX{0.f}; //!< Spacing in 0..1 range between bars horizontally. Parsed from bars_spacing_x.
    float m_spacingY{0.f}; //!< Spacing in 0..1 range between rectangles vertically. Parsed from bars_spacing_y.

    float m_rotation{0.f}; //!< Rotation in radians. Parsed from bars_rot (as degrees).

    float m_scaleX{1.f}; //!< Scale by X axis. Parsed from bars_sx.
    float m_scaleY{1.f}; //!< Scale by Y axis. Parsed from bars_sy.

    float m_smooth{0.f};   //!< Bars motion smoothing factor 0..1. Parsed from bars_smooth.
    float m_rounding{0.f}; //!< Bars end rounding factor 0..1. Parsed from bars_rounding.

    float m_offsetX{0.f}; //!< Bars x offset -1..0..1. Parsed from bars_x.
    float m_offsetY{0.f}; //!< Bars y offset -1..0..1. Parsed from bars_y.

    float m_sensitivityX{1.f};     //!< Bars sensitivity related to frequency amplitude. Parsed from bars_sensitivity.
    float m_bassSensitivityX{1.f}; //!< Bars sensitivity related to frequency amplitude. Parsed from bars_bass_sensitivity.

    uint32_t m_colorTop{0xFFFFFFFF};                  //!< Top bars color in 0xAARRGGBB format. Parsed from bars_color_t.
    uint32_t m_colorBottom{0xFFFFFFFF};               //!< Bottom bars color in 0xAARRGGBB format. Parsed from bars_color_b.
    std::optional<uint32_t> m_colorLeft{0xFFFFFFFF};  //!< Optional left bars color overlay in 0xAARRGGBB format. Parsed from bars_color_l.
    std::optional<uint32_t> m_colorRight{0xFFFFFFFF}; //!< Optional right bars color overlay in 0xAARRGGBB format. Parsed from bars_color_r.

    float m_threshold{0.f};                                    //!< For non-reflected bars 0..1 position where threshold color starts. Parsed from bars_thr.
    uint32_t m_thresholdColorTop{0xFFFFFFFF};                  //!< For non-reflected bars: top threshold color in 0xAARRGGBB format. For reflected bars: 2nd half top color in 0xAARRGGBB format. Parsed from bars_thr_color_t.
    uint32_t m_thresholdColorBottom{0xFFFFFFFF};               //!< For non-reflected bars: bottom threshold color in 0xAARRGGBB format. For reflected bars: 2nd half bottom color in 0xAARRGGBB format. Parsed from bars_thr_color_b.
    std::optional<uint32_t> m_thresholdColorLeft{0xFFFFFFFF};  //!< For non-reflected bars: optional left threshold color overlay in 0xAARRGGBB format. For reflected bars: 2nd half optional left color overlay in 0xAARRGGBB format. Parsed from bars_thr_color_l.
    std::optional<uint32_t> m_thresholdColorRight{0xFFFFFFFF}; //!< For non-reflected bars: optional right threshold color overlay in 0xAARRGGBB format. For reflected bars: 2nd half optional right color overlay in 0xAARRGGBB format. Parsed from bars_thr_color_r.

    float m_peakHeight{0.f};       //!< Bar peak height. Parsed from bars_peak_height.
    float m_peakSpacing{0.f};      //!< Bar peak spacing. Parsed from bars_peak_spacing.
    float m_peakDelay{0.f};        //!< Bar peak fall delay in seconds. Parsed from bars_peak_delay.
    float m_peakAcceleration{1.f}; //!< Bar peak fall acceleration. Parsed from bars_peak_accel.

    uint32_t m_peakColorTop{0xFFFFFFFF};                  //!< Top bars peak color in 0xAARRGGBB format. Parsed from bars_peak_color_t.
    uint32_t m_peakColorBottom{0xFFFFFFFF};               //!< Bottom bars color in 0xAARRGGBB format. Parsed from bars_peak_color_b.
    std::optional<uint32_t> m_peakColorLeft{0xFFFFFFFF};  //!< Optional left bars peak color overlay in 0xAARRGGBB format. Parsed from bars_peak_color_l.
    std::optional<uint32_t> m_peakColorRight{0xFFFFFFFF}; //!< Optional right bars peak color overlay in 0xAARRGGBB format. Parsed from bars_peak_color_r.
};

} // namespace MilkdropPreset
} // namespace libprojectM
