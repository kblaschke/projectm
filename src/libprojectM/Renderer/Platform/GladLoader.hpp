#pragma once

#include "glad/gl.h"


#include <mutex>

namespace libprojectM {
namespace Renderer {
namespace Platform {

/**
 * @brief Helper for GLAD loading and projectM-specific GL requirements checks.
 *
 * @note This loader is a process-singleton.
 * @note The GLResolver needs to be initialized first, before using this loader.
 */
class GladLoader
{
public:
    GladLoader() = default;
    ~GladLoader() = default;

    GladLoader(const GladLoader&) = delete;
    auto operator=(const GladLoader&) -> GladLoader& = delete;
    GladLoader(GladLoader&&) = delete;
    auto operator=(GladLoader&&) -> GladLoader& = delete;

    /**
     * @brief Load GLAD using the GLResolver. Loading is performed only if GLAD has not been loaded already.
     * @param context The GladGLContext structure to populate.
     * @return true if GLAD was loaded successfully, or was already loaded.
     */
    static auto GetGladGlContext(GladGLContext& context) -> bool;

private:
    /**
     * @brief Check if the required GL context requirements are met.
     * @param checkGLES If true, checks for a GLES 3.0 context, otherwise for a 3.3 Core profile.
     * @param context The GladGLContext structure to populate.
     * @return true if the check was successful and the context structure was populated.
     */
    static auto CheckGLRequirements(bool checkGLES, GladGLContext& context) -> bool;
};

} // namespace Platform
} // namespace Renderer
} // namespace libprojectM
