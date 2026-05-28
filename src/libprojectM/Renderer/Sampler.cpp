#include "Sampler.hpp"

namespace libprojectM {
namespace Renderer {

Sampler::Sampler(const GLint wrapMode, const GLint filterMode, const GladGLContext& glContext)
    : m_wrapMode(wrapMode)
    , m_filterMode(filterMode)
    , m_glContext(glContext)
{
    m_glContext.GenSamplers(1, &m_samplerId);
    m_glContext.SamplerParameteri(m_samplerId, GL_TEXTURE_MIN_FILTER, filterMode);
    m_glContext.SamplerParameteri(m_samplerId, GL_TEXTURE_MAG_FILTER, filterMode);
    m_glContext.SamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_S, wrapMode);
    m_glContext.SamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_T, wrapMode);
}


Sampler::~Sampler()
{
    m_glContext.DeleteSamplers(1, &m_samplerId);
}

void Sampler::Bind(GLuint unit) const
{
    m_glContext.BindSampler(unit, m_samplerId);
}

void Sampler::Unbind(GLuint unit) const
{
    m_glContext.BindSampler(unit, 0);
}

void Sampler::Unbind(GLuint unit, const GladGLContext& glContext)
{
    glContext.BindSampler(unit, 0);
}

auto Sampler::WrapMode() const -> GLint
{
    return m_wrapMode;
}

void Sampler::WrapMode(GLint wrapMode)
{
    if (wrapMode == m_wrapMode)
    {
        return;
    }

    m_wrapMode = wrapMode;

    m_glContext.SamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_S, wrapMode);
    m_glContext.SamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_T, wrapMode);
}

auto Sampler::FilterMode() const -> GLint
{
    return m_filterMode;
}

void Sampler::FilterMode(GLint filterMode)
{
    if (filterMode == m_filterMode)
    {
        return;
    }

    m_filterMode = filterMode;

    m_glContext.SamplerParameteri(m_samplerId, GL_TEXTURE_MIN_FILTER, filterMode);
    m_glContext.SamplerParameteri(m_samplerId, GL_TEXTURE_MAG_FILTER, filterMode);
}

} // namespace Renderer
} // namespace libprojectM
