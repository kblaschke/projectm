#pragma once

#include "Preset.hpp"

#include <memory>

namespace libprojectM {

class PresetFactory
{

public:
    PresetFactory() = default;

    virtual ~PresetFactory() = default;

    /**
     * @brief Returns the protocol portion for the given URL
     *
     * Some implementation may hand over "file:///path/to/preset" as the preset filename. This method
     * takes care of cutting of the URL portion and return the path. Internally, it can be used to load
     * the default idle preset via the "idle://" URL.
     *
     * @param url The URL to parse
     * @param path [out] The path portion of the URL, basically everything after the "://" delimiter.
     * @return The protocol, e.g. "file"
     */
    static std::string Protocol(const std::string& url, std::string& path);

    /**
     * @brief Constructs a new preset from a local file
     * @param filename The preset filename
     * @param renderContext The projectM preset render context.
     * @returns A valid preset object
     */
    virtual std::unique_ptr<Preset> LoadPresetFromFile(const std::string& filename, const Renderer::RenderContext& renderContext) = 0;

    /**
     * @brief Constructs a new preset from a stream
     * @param data The preset data stream
     * @param renderContext The projectM preset render context.
     * @returns A valid preset object
     */
    virtual std::unique_ptr<Preset> LoadPresetFromStream(std::istream& data, const Renderer::RenderContext& renderContext) = 0;

    /**
     * Returns a space separated list of supported extensions
     * @return A space separated list of supported extensions
     */
    virtual std::string supportedExtensions() const = 0;
};

} // namespace libprojectM
