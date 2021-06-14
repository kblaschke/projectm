#include "IdlePreset.hpp"

#include "MilkdropPreset.hpp"

#include "IdlePresetResources.h"

#include <sstream>
#include <string>

const std::string IdlePresets::IDLE_PRESET_NAME
    ("Geiss & Sperl - Feedback (projectM idle HDR mix).milk");

std::string IdlePresets::presetText()
{
    return { idlePresetText.data(), idlePresetText.size() };
}

std::unique_ptr<Preset>
IdlePresets::allocate(MilkdropPresetFactory* factory, const std::string& name, PresetOutputs* presetOutputs)
{
    
    if (name == IDLE_PRESET_NAME)
    {
        std::istringstream in(presetText());
        return std::unique_ptr<Preset>(new MilkdropPreset(factory, in, IDLE_PRESET_NAME, presetOutputs));
    }
    else
    {
        return std::unique_ptr<Preset>();
    }
}
