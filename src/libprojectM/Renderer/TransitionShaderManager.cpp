#include "Renderer/TransitionShaderManager.hpp"

#include "BuiltInTransitionsResources.hpp"
#include "Logging.hpp"

namespace libprojectM {
namespace Renderer {

TransitionShaderManager::TransitionShaderManager(const GladGLContext& glContext)
    : m_transitionShaders({CompileTransitionShader(kTransitionShaderBuiltInCircle, glContext),
                           CompileTransitionShader(kTransitionShaderBuiltInPlasma, glContext),
                           CompileTransitionShader(kTransitionShaderBuiltInSimpleBlend, glContext),
                           CompileTransitionShader(kTransitionShaderBuiltInSweep, glContext),
                           CompileTransitionShader(kTransitionShaderBuiltInWarp, glContext),
                           CompileTransitionShader(kTransitionShaderBuiltInZoomBlur, glContext)})
    , m_mersenneTwister(m_randomDevice())
{
}

auto TransitionShaderManager::RandomTransition() -> std::shared_ptr<Shader>
{
    if (m_transitionShaders.empty())
    {
        return {};
    }

    return m_transitionShaders.at(m_mersenneTwister() % m_transitionShaders.size());
}

auto TransitionShaderManager::CompileTransitionShader(const std::string& shaderBodyCode, const GladGLContext& glContext) -> std::shared_ptr<Shader>
{
    std::string versionHeader;
    if (glContext.ES_VERSION_3_0)
    {
        // GLES also requires a precision specifier for variables and 3D samplers
        versionHeader = "#version 300 es\n\nprecision mediump float;\nprecision mediump sampler3D;\n";
    }
    else
    {
        versionHeader = "#version 330\n\n";
    }

    std::string fragmentShaderSource = versionHeader;
    fragmentShaderSource.append(kTransitionShaderHeader);
    fragmentShaderSource.append("\n");
    fragmentShaderSource.append(shaderBodyCode);
    fragmentShaderSource.append("\n");
    fragmentShaderSource.append(kTransitionShaderMain);

    try
    {
        auto transitionShader = std::make_shared<Shader>(glContext);
        transitionShader->CompileProgram(versionHeader + kTransitionVertexShader, fragmentShaderSource);
        return transitionShader;
    }
    catch (const ShaderException& ex)
    {
        LOG_ERROR("Failed to compile transition shader: " + ex.message());
        return {};
    }
}

} // namespace Renderer
} // namespace libprojectM
