#include "Filters.hpp"

#include <Renderer/BlendMode.hpp>

using libprojectM::Renderer::BlendMode;

namespace libprojectM {
namespace MilkdropPreset {

Filters::Filters(const PresetState& presetState)
    : m_presetState(presetState)
    , m_filterMesh(Renderer::VertexBufferUsage::StaticDraw, presetState.renderContext.glContext)
{
    m_filterMesh.SetRenderPrimitiveType(Renderer::Mesh::PrimitiveType::TriangleStrip);
    m_filterMesh.SetVertexCount(4);
}

void Filters::Draw()
{
    const auto& glContext = m_presetState.renderContext.glContext;

    UpdateMesh();

    if (m_viewportWidth == 0 || m_viewportHeight == 0)
    {
        return;
    }

    BlendMode::SetBlendActive(true, glContext);

    auto shader = m_presetState.untexturedShader.lock();
    shader->Bind();
    shader->SetUniformMat4x4("vertex_transformation", PresetState::orthogonalProjection);
    shader->SetUniformFloat("vertex_point_size", 1.0f);

    glContext.VertexAttrib4f(1, 1.0, 1.0, 1.0, 1.0);

    if (m_presetState.brighten)
    {
        Brighten();
    }
    if (m_presetState.darken)
    {
        Darken();
    }
    if (m_presetState.solarize)
    {
        Solarize();
    }
    if (m_presetState.invert)
    {
        Invert();
    }

    Renderer::Mesh::Unbind(glContext);
    Renderer::Shader::Unbind(glContext);

    BlendMode::SetBlendActive(false, glContext);
}


void Filters::Brighten()
{
    const auto& glContext = m_presetState.renderContext.glContext;

    BlendMode::SetBlendFunction(BlendMode::Function::OneMinusDestinationColor, BlendMode::Function::Zero, glContext);
    m_filterMesh.Draw();
    BlendMode::SetBlendFunction(BlendMode::Function::Zero, BlendMode::Function::DestinationColor, glContext);
    m_filterMesh.Draw();
    BlendMode::SetBlendFunction(BlendMode::Function::OneMinusDestinationColor, BlendMode::Function::Zero, glContext);
    m_filterMesh.Draw();
}

void Filters::Darken()
{
    const auto& glContext = m_presetState.renderContext.glContext;

    BlendMode::SetBlendFunction(BlendMode::Function::Zero, BlendMode::Function::DestinationColor, glContext);
    m_filterMesh.Draw();
}

void Filters::Solarize()
{
    const auto& glContext = m_presetState.renderContext.glContext;

    BlendMode::SetBlendFunction(BlendMode::Function::Zero, BlendMode::Function::OneMinusDestinationColor, glContext);
    m_filterMesh.Draw();
    BlendMode::SetBlendFunction(BlendMode::Function::DestinationColor, BlendMode::Function::One, glContext);
    m_filterMesh.Draw();
}

void Filters::Invert()
{
    const auto& glContext = m_presetState.renderContext.glContext;

    BlendMode::SetBlendFunction(BlendMode::Function::OneMinusDestinationColor, BlendMode::Function::Zero, glContext);
    m_filterMesh.Draw();
}

void Filters::UpdateMesh()
{
    if (m_viewportWidth == m_presetState.renderContext.viewportSizeX &&
        m_viewportHeight == m_presetState.renderContext.viewportSizeY)
    {
        return;
    }

    m_viewportWidth = m_presetState.renderContext.viewportSizeX;
    m_viewportHeight = m_presetState.renderContext.viewportSizeY;

    float const fOnePlusInvWidth = 1.0f + 1.0f / static_cast<float>(m_viewportWidth);
    float const fOnePlusInvHeight = 1.0f + 1.0f / static_cast<float>(m_viewportHeight);
    m_filterMesh.Vertices().Set({{-fOnePlusInvWidth, fOnePlusInvHeight},
                                 {fOnePlusInvWidth, fOnePlusInvHeight},
                                 {-fOnePlusInvWidth, -fOnePlusInvHeight},
                                 {fOnePlusInvWidth, -fOnePlusInvHeight}});

    m_filterMesh.Update();
}

} // namespace MilkdropPreset
} // namespace libprojectM
