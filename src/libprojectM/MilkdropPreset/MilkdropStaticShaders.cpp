#include "MilkdropStaticShaders.hpp"

#include <string>

namespace libprojectM {
namespace MilkdropPreset {

// Shader program string constants
#include "MilkdropStaticShaders_Content.hpp"

MilkdropStaticShaders::MilkdropStaticShaders(const bool useGLES)
    : m_useGLES(useGLES)
{
    if (m_useGLES)
    {
        // If GLES is enabled, use the embedded specification language variant.
        m_versionHeader = "#version 300 es";
        m_GLSLGeneratorVersion = M4::GLSLGenerator::Version::Version_300_ES;
    }
    else
    {
        m_versionHeader = "#version 330";
        m_GLSLGeneratorVersion = M4::GLSLGenerator::Version::Version_330;
    }
}

std::string MilkdropStaticShaders::AddVersionHeader(const std::string& shader_text) const
{
    return m_versionHeader + "\n" + shader_text;
}

#define DECLARE_SHADER_ACCESSOR(name)              \
    std::string MilkdropStaticShaders::Get##name()       \
    {                                              \
        return AddVersionHeader(k##name##Glsl330); \
    }

#define DECLARE_SHADER_ACCESSOR_NO_HEADER(name) \
    std::string MilkdropStaticShaders::Get##name()    \
    {                                           \
        return k##name##Glsl330;                \
    }

#include "MilkdropStaticShaders_Implementations.hpp"

} // namespace MilkdropPreset
} // namespace libprojectM