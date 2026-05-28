#include "Border.hpp"

#include "PerFrameContext.hpp"

#include <Renderer/BlendMode.hpp>

namespace libprojectM {
namespace MilkdropPreset {

Border::Border(PresetState& presetState)
    : m_presetState(presetState)
    , m_borderMesh(Renderer::VertexBufferUsage::StreamDraw, presetState.renderContext.glContext)
{
    m_borderMesh.SetRenderPrimitiveType(Renderer::Mesh::PrimitiveType::Triangles);
    m_borderMesh.SetVertexCount(8);

    m_borderMesh.Indices().Set({
    {
        0, 1, 4,
        1, 4, 5,
        2, 3, 6,
        3, 7, 6,
        2, 0, 6,
        0, 4, 6,
        3, 7, 5,
        1, 3, 5
    }});
}

void Border::Draw(const PerFrameContext& presetPerFrameContext)
{
    const auto& glContext = m_presetState.renderContext.glContext;

    // Draw Borders
    float const outerBorderSize = static_cast<float>(*presetPerFrameContext.ob_size);
    float const innerBorderSize = static_cast<float>(*presetPerFrameContext.ib_size);

    // No additive drawing for borders
    Renderer::BlendMode::Set(true, Renderer::BlendMode::Function::SourceAlpha, Renderer::BlendMode::Function::OneMinusSourceAlpha, glContext);

    auto shader = m_presetState.untexturedShader.lock();
    shader->Bind();
    shader->SetUniformMat4x4("vertex_transformation", PresetState::orthogonalProjection);
    shader->SetUniformFloat("vertex_point_size", 1.0f);

    m_borderMesh.Bind();

    for (int border = 0; border < 2; border++)
    {
        float r = (border == 0) ? static_cast<float>(*presetPerFrameContext.ob_r) : static_cast<float>(*presetPerFrameContext.ib_r);
        float g = (border == 0) ? static_cast<float>(*presetPerFrameContext.ob_g) : static_cast<float>(*presetPerFrameContext.ib_g);
        float b = (border == 0) ? static_cast<float>(*presetPerFrameContext.ob_b) : static_cast<float>(*presetPerFrameContext.ib_b);
        float a = (border == 0) ? static_cast<float>(*presetPerFrameContext.ob_a) : static_cast<float>(*presetPerFrameContext.ib_a);

        if (a > 0.001f)
        {
            glContext.VertexAttrib4f(1, r, g, b, a);

            float innerRadius = (border == 0) ? 1.0f - outerBorderSize : 1.0f - outerBorderSize - innerBorderSize;
            float outerRadius = (border == 0) ? 1.0f : 1.0f - outerBorderSize;

             m_borderMesh.Vertices().Set({{outerRadius, outerRadius},
                                                     {outerRadius, -outerRadius},
                                                     {-outerRadius, outerRadius},
                                                     {-outerRadius, -outerRadius},
                                                     {innerRadius, innerRadius},
                                                     {innerRadius, -innerRadius},
                                                     {-innerRadius, innerRadius},
                                                     {-innerRadius, -innerRadius}});

            m_borderMesh.Update();
            m_borderMesh.Draw();
        }
    }

    Renderer::Mesh::Unbind(glContext);
    Renderer::Shader::Unbind(glContext);
    Renderer::BlendMode::SetBlendActive(false, glContext);
}

} // namespace MilkdropPreset
} // namespace libprojectM
