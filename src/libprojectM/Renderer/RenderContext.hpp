/**
* @file RenderContext.hpp
* @brief A class holding per-frame render values for use in preset rendering.
*/
#pragma once

#include <glad/gl.h>
#include <memory>

#include <projectM-4/projectM_cxx_export.h>

namespace libprojectM {
namespace Renderer {

class ShaderCache;
class TextureManager;

/**
 * @brief Holds all global data of the current rendering context, which can change from frame to frame.
 */
class PROJECTM_CXX_EXPORT RenderContext
{
public:
    RenderContext(const GladGLContext& glContextInit, std::shared_ptr<TextureManager> textureManagerInit, ShaderCache& shaderCacheInit)
        : glContext(glContextInit)
        , textureManager(textureManagerInit)
        , shaderCache(shaderCacheInit)
    {
    }

    float time{0.0f};          //!< Time since the preset started, in seconds.
    int frame{0};              //!< Frames rendered so far.
    float fps{0.0f};           //!< Frames per second.
    float progress{0.0f};      //!< Preset progress.
    float blendProgress{0.0f}; //!< Preset transition/blending progress.
    int viewportSizeX{0};      //!< Horizontal viewport size in pixels
    int viewportSizeY{0};      //!< Vertical viewport size in pixels
    float aspectX{1.0};        //!< X aspect ratio.
    float aspectY{1.0};        //!< Y aspect ratio.
    float invAspectX{1.0};     //!< Inverse X aspect ratio.
    float invAspectY{1.0};     //!< Inverse Y aspect ratio.

    int perPixelMeshX{64}; //!< Per-pixel/per-vertex mesh X resolution.
    int perPixelMeshY{48}; //!< Per-pixel/per-vertex mesh Y resolution.

    float texelOffsetX{0.0f}; //!< Horizontal texel offset in the warp shader.
    float texelOffsetY{0.0f}; //!< Vertical texel offset in the warp shader.

    const GladGLContext& glContext;                 //!< The OpenGL context used for rendering.
    std::shared_ptr<TextureManager> textureManager; //!< Holds all loaded textures for shader access.
    ShaderCache& shaderCache;                       //!< The shader cache of this projectM instance.
};

} // namespace Renderer
} // namespace libprojectM
