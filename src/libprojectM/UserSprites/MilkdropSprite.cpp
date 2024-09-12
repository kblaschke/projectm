#include "MilkdropSprite.hpp"

#include "PresetFileParser.hpp"

#include "Renderer/TextureManager.hpp"

#include <cstdlib>
#include <locale>
#include <sstream>
#include <string>

#define REG_VAR(var) \
    var = projectm_eval_context_register_variable(spriteCodeContext, #var);

namespace libprojectM {
namespace UserSprites {

MilkdropSprite::MilkdropSprite()
{
    RenderItem::Init();
}

MilkdropSprite::~MilkdropSprite()
{
}

void MilkdropSprite::InitVertexAttrib()
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedPoint), reinterpret_cast<void*>(offsetof(TexturedPoint, x))); // Position
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(TexturedPoint), reinterpret_cast<void*>(offsetof(TexturedPoint, r))); // Color
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedPoint), reinterpret_cast<void*>(offsetof(TexturedPoint, u))); // Texture coordinate
}

void MilkdropSprite::Init(const std::string& spriteData, const Renderer::RenderContext& renderContext)
{
    PresetFileParser parser;
    std::stringstream spriteDataStream(spriteData);
    if (!parser.Read(spriteDataStream))
    {
        throw SpriteException("Error reading sprite data.");
    }

    // FIRST try 'colorkey_lo' (for backwards compatibility) and then try 'colorkey'
    auto colorKey = parser.GetString("colorkey", parser.GetString("colorkey_lo", ""));

    // Convert hexadecimal color key to uint32
    if (colorKey.length() >= 8 && colorKey[0] == '0' && (colorKey[1] == 'x' || colorKey[1] == 'X'))
    {
        char* end = colorKey.data() + 10;
        m_colorKey = std::strtoul(colorKey.data() + 2, &end, 16);
    }
    else
    {
        char* end = colorKey.data() + colorKey.length();
        m_colorKey = std::strtoul(colorKey.data(), &end, 10);
    }

    // projectM addition. Use "enable_colorkey=0" to disable color keying.
    m_enableColorKey = parser.GetBool("enable_colorkey", true);

    // Load and compile code
    auto initCode = parser.GetCode("init_");
    m_codeContext.RunInitCode(initCode, renderContext);

    auto perFrameCode = parser.GetCode("code_");
    if (!perFrameCode.empty())
    {
        m_codeContext.perFrameCodeHandle = projectm_eval_code_compile(m_codeContext.spriteCodeContext, perFrameCode.c_str());
        if (!m_codeContext.perFrameCodeHandle)
        {
            int errorLine{};
            int errorColumn{};
            auto* errorMessage = projectm_eval_get_error(m_codeContext.spriteCodeContext, &errorLine, &errorColumn);

            throw SpriteException("Error compiling sprite per-frame code:" + std::string(errorMessage) + " (Line " + std::to_string(errorLine) + ", column " + std::to_string(errorColumn) + ")");
        }
    }

    auto imageName = parser.GetString("img", "");
    std::transform(imageName.begin(), imageName.end(), imageName.begin(),
                   [](unsigned char c){ return std::tolower(c); }
    );

    std::locale const loc;
    if (imageName.length() >= 6 &&
        imageName.substr(0, 4) == "rand" && std::isdigit(imageName.at(4), loc) && std::isdigit(imageName.at(5), loc))
    {
        m_texture = renderContext.textureManager->GetRandomTexture(imageName);
    }
    else
    {
        m_texture = renderContext.textureManager->GetTexture(imageName);
    }
}

void MilkdropSprite::Draw(const Audio::FrameAudioData& audioData,
                          const Renderer::RenderContext& renderContext,
                          uint32_t outputFramebufferObject,
                          PresetList presets)
{
    m_codeContext.RunPerFrameCode(audioData, renderContext);

    m_spriteDone = *m_codeContext.done != 0.0;
    bool burnIn = *m_codeContext.burn != 0.0;

    Quad vertices{};


    // Burn-in the sprite to both presets
    if (burnIn)
    {
        for (const auto& preset : presets)
        {
            preset.get()->BindFramebuffer();
        }
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, static_cast<GLuint>(outputFramebufferObject));

    // Draw the actual sprite.


}

auto MilkdropSprite::Done() const -> bool
{
    return m_spriteDone;
}

MilkdropSprite::CodeContext::CodeContext()
    : spriteCodeContext(projectm_eval_context_create(nullptr, nullptr))
{
}

MilkdropSprite::CodeContext::~CodeContext()
{
    projectm_eval_context_destroy(spriteCodeContext);

    spriteCodeContext = nullptr;
    perFrameCodeHandle = nullptr;
}

void MilkdropSprite::CodeContext::RegisterBuiltinVariables()
{
    projectm_eval_context_reset_variables(spriteCodeContext);

    // Input variables
    REG_VAR(time);
    REG_VAR(frame);
    REG_VAR(fps);
    REG_VAR(progress);
    REG_VAR(bass);
    REG_VAR(mid);
    REG_VAR(treb);
    REG_VAR(bass_att);
    REG_VAR(mid_att);
    REG_VAR(treb_att);

    // Output variables
    REG_VAR(done);
    REG_VAR(burn);
    REG_VAR(x);
    REG_VAR(y);
    REG_VAR(sx);
    REG_VAR(sy);
    REG_VAR(rot);
    REG_VAR(flipx);
    REG_VAR(flipy);
    REG_VAR(repeatx);
    REG_VAR(repeaty);
    REG_VAR(blendmode);
    REG_VAR(r);
    REG_VAR(g);
    REG_VAR(b);
    REG_VAR(a);
}

void MilkdropSprite::CodeContext::RunInitCode(const std::string& initCode, const Renderer::RenderContext& renderContext)
{
    RegisterBuiltinVariables();

    // Set default values of output variables:
    // (by not setting these every frame, we allow the values to persist from frame-to-frame.)
    *x = 0.5;
    *y = 0.5;
    *sx = 1.0;
    *sy = 1.0;
    *repeatx = 1.0;
    *repeaty = 1.0;
    *rot = 0.0;
    *flipx = 0.0;
    *flipy = 0.0;
    *r = 1.0;
    *g = 1.0;
    *b = 1.0;
    *a = 1.0;
    *blendmode = 0.0;
    *done = 0.0;
    *burn = 1.0;

    if (initCode.empty())
    {
        return;
    }

    // Only time and frame values are passed to the init code.
    *time = renderContext.time;
    *frame = renderContext.frame;

    auto* initCodeHandle = projectm_eval_code_compile(spriteCodeContext, initCode.c_str());
    if (initCodeHandle == nullptr)
    {
        int errorLine{};
        int errorColumn{};
        const auto* errorMessage = projectm_eval_get_error(spriteCodeContext, &errorLine, &errorColumn);

        throw SpriteException("Error compiling sprite init code:" + std::string(errorMessage) + " (Line " + std::to_string(errorLine) + ", column " + std::to_string(errorColumn) + ")");
    }

    projectm_eval_code_execute(initCodeHandle);
    projectm_eval_code_destroy(initCodeHandle);
}

void MilkdropSprite::CodeContext::RunPerFrameCode(const Audio::FrameAudioData& audioData, const Renderer::RenderContext& renderContext)
{
    // If there's no per-frame code, e.g. with static sprites, just skip it.
    if (perFrameCodeHandle == nullptr)
    {
        return;
    }

    // Fill in input variables
    *time = renderContext.time;
    *frame = renderContext.frame;
    *fps = renderContext.fps;
    *progress = renderContext.blendProgress;
    *bass = audioData.bass;
    *mid = audioData.mid;
    *treb = audioData.treb;
    *bass_att = audioData.bassAtt;
    *mid_att = audioData.midAtt;
    *treb_att = audioData.trebAtt;

    // Run the code
    projectm_eval_code_execute(perFrameCodeHandle);
}

} // namespace UserSprites
} // namespace libprojectM
