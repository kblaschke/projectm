#include "Shader.hpp"

#include <Logging.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

namespace libprojectM {
namespace Renderer {

Shader::Shader(const GladGLContext& glContext)
    : m_shaderProgram(glContext.CreateProgram())
    , m_glContext(glContext)
{
}

Shader::~Shader()
{
    if (m_shaderProgram)
    {
        m_glContext.DeleteProgram(m_shaderProgram);
    }
}

void Shader::CompileProgram(const std::string& vertexShaderSource,
                            const std::string& fragmentShaderSource)
{
    auto vertexShader = CompileShader(vertexShaderSource, GL_VERTEX_SHADER);
    auto fragmentShader = CompileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    m_glContext.AttachShader(m_shaderProgram, vertexShader);
    m_glContext.AttachShader(m_shaderProgram, fragmentShader);

    m_glContext.LinkProgram(m_shaderProgram);

    // Shader objects are no longer needed after linking, free the memory.
    m_glContext.DetachShader(m_shaderProgram, vertexShader);
    m_glContext.DetachShader(m_shaderProgram, fragmentShader);
    m_glContext.DeleteShader(vertexShader);
    m_glContext.DeleteShader(fragmentShader);

    GLint programLinked;
    m_glContext.GetProgramiv(m_shaderProgram, GL_LINK_STATUS, &programLinked);
    if (programLinked == GL_TRUE)
    {
        return;
    }

    GLint infoLogLength{};
    m_glContext.GetProgramiv(m_shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
    std::vector<char> message(infoLogLength + 1);
    m_glContext.GetProgramInfoLog(m_shaderProgram, infoLogLength, nullptr, message.data());

    std::string linkError = "[Shader] Error linking compiled shader program: " + std::string(message.data());
    LOG_ERROR(linkError);
    LOG_DEBUG("[Shader] Vertex shader source: " + vertexShaderSource);
    LOG_DEBUG("[Shader] Fragment shader source: " + fragmentShaderSource);
    throw ShaderException(linkError);
}

bool Shader::Validate(std::string& validationMessage) const
{
    GLint result{GL_FALSE};
    int infoLogLength;

    m_glContext.ValidateProgram(m_shaderProgram);

    m_glContext.GetProgramiv(m_shaderProgram, GL_VALIDATE_STATUS, &result);
    m_glContext.GetProgramiv(m_shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0)
    {
        std::vector<char> validationErrorMessage(infoLogLength + 1);
        m_glContext.GetProgramInfoLog(m_shaderProgram, infoLogLength, nullptr, validationErrorMessage.data());
        validationMessage = std::string(validationErrorMessage.data());
    }

    return result;
}

void Shader::Bind() const
{
    if (m_shaderProgram > 0)
    {
        m_glContext.UseProgram(m_shaderProgram);
    }
}

void Shader::Unbind() const
{
    m_glContext.UseProgram(0);
}

void Shader::Unbind(const GladGLContext& glContext)
{
    glContext.UseProgram(0);
}

void Shader::SetUniformFloat(const char* uniform, float value) const
{
    auto location = m_glContext.GetUniformLocation(m_shaderProgram, uniform);
    if (location < 0)
    {
        return;
    }
    m_glContext.Uniform1fv(location, 1, &value);
}

void Shader::SetUniformInt(const char* uniform, int value) const
{
    auto location = m_glContext.GetUniformLocation(m_shaderProgram, uniform);
    if (location < 0)
    {
        return;
    }
    m_glContext.Uniform1iv(location, 1, &value);
}

void Shader::SetUniformFloat2(const char* uniform, const glm::vec2& values) const
{
    auto location = m_glContext.GetUniformLocation(m_shaderProgram, uniform);
    if (location < 0)
    {
        return;
    }
    m_glContext.Uniform2fv(location, 1, glm::value_ptr(values));
}

void Shader::SetUniformInt2(const char* uniform, const glm::ivec2& values) const
{
    auto location = m_glContext.GetUniformLocation(m_shaderProgram, uniform);
    if (location < 0)
    {
        return;
    }
    m_glContext.Uniform2iv(location, 1, glm::value_ptr(values));
}

void Shader::SetUniformFloat3(const char* uniform, const glm::vec3& values) const
{
    auto location = m_glContext.GetUniformLocation(m_shaderProgram, uniform);
    if (location < 0)
    {
        return;
    }
    m_glContext.Uniform3fv(location, 1, glm::value_ptr(values));
}

void Shader::SetUniformInt3(const char* uniform, const glm::ivec3& values) const
{
    auto location = m_glContext.GetUniformLocation(m_shaderProgram, uniform);
    if (location < 0)
    {
        return;
    }
    m_glContext.Uniform3iv(location, 1, glm::value_ptr(values));
}

void Shader::SetUniformFloat4(const char* uniform, const glm::vec4& values) const
{
    auto location = m_glContext.GetUniformLocation(m_shaderProgram, uniform);
    if (location < 0)
    {
        return;
    }
    m_glContext.Uniform4fv(location, 1, glm::value_ptr(values));
}

void Shader::SetUniformInt4(const char* uniform, const glm::ivec4& values) const
{
    auto location = m_glContext.GetUniformLocation(m_shaderProgram, uniform);
    if (location < 0)
    {
        return;
    }
    m_glContext.Uniform4iv(location, 1, glm::value_ptr(values));
}

void Shader::SetUniformMat3x4(const char* uniform, const glm::mat3x4& values) const
{
    auto location = m_glContext.GetUniformLocation(m_shaderProgram, uniform);
    if (location < 0)
    {
        return;
    }
    m_glContext.UniformMatrix3x4fv(location, 1, GL_FALSE, glm::value_ptr(values));
}

void Shader::SetUniformMat4x4(const char* uniform, const glm::mat4x4& values) const
{
    auto location = m_glContext.GetUniformLocation(m_shaderProgram, uniform);
    if (location < 0)
    {
        return;
    }
    m_glContext.UniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(values));
}

GLuint Shader::CompileShader(const std::string& source, GLenum type)
{
    GLint shaderCompiled{};

    auto shader = m_glContext.CreateShader(type);
    const auto* shaderSourceCStr = source.c_str();
    m_glContext.ShaderSource(shader, 1, &shaderSourceCStr, nullptr);

    m_glContext.CompileShader(shader);

    m_glContext.GetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompiled);
    if (shaderCompiled == GL_TRUE)
    {
        return shader;
    }

    GLint infoLogLength{};
    m_glContext.GetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
    std::vector<char> message(infoLogLength + 1);
    m_glContext.GetShaderInfoLog(shader, infoLogLength, nullptr, message.data());
    m_glContext.DeleteShader(shader);

    std::string compileError = "[Shader] Error compiling " + std::string(type == GL_VERTEX_SHADER ? "vertex" : "fragment") + " shader: " + std::string(message.data());
    LOG_ERROR(compileError);
    LOG_DEBUG("[Shader] Failed source: " + source);
    throw ShaderException(compileError);
}

auto Shader::GetShaderLanguageVersion(const GladGLContext& glContext) -> Shader::GlslVersion
{
    const char* shaderLanguageVersion = reinterpret_cast<const char*>(glContext.GetString(GL_SHADING_LANGUAGE_VERSION));

    if (shaderLanguageVersion == nullptr)
    {
        return {};
    }

    std::string shaderLanguageVersionString(shaderLanguageVersion);

    // Some OpenGL implementations add non-standard-conforming text in front, e.g. WebGL, which returns "OpenGL ES GLSL ES 3.00 ..."
    // Find the first digit and start there.
    auto firstDigit = shaderLanguageVersionString.find_first_of("0123456789");
    if (firstDigit != std::string::npos && firstDigit != 0)
    {
        shaderLanguageVersionString = shaderLanguageVersionString.substr(firstDigit);
    }

    // Cut off the vendor-specific information, if any
    auto spacePos = shaderLanguageVersionString.find(' ');
    if (spacePos != std::string::npos)
    {
        shaderLanguageVersionString.resize(spacePos);
    }

    auto dotPos = shaderLanguageVersionString.find('.');
    if (dotPos == std::string::npos)
    {
        return {};
    }

    int versionMajor = std::stoi(shaderLanguageVersionString.substr(0, dotPos));
    int versionMinor = std::stoi(shaderLanguageVersionString.substr(dotPos + 1));

    return {versionMajor, versionMinor};
}

} // namespace Renderer
} // namespace libprojectM
