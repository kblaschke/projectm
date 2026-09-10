#include "UserSprites/SpriteManager.hpp"

#include "UserSprites/Factory.hpp"
#include "UserSprites/SpriteException.hpp"

#include <Logging.hpp>

#include <Renderer/Shader.hpp>

#include <algorithm>

namespace libprojectM {
namespace UserSprites {

auto SpriteManager::Spawn(const std::string& type,
                          const std::string& spriteData,
                          const Renderer::RenderContext& renderContext) -> uint32_t
{
    // If user set the limit to zero, don't bother.
    if (m_spriteSlots == 0)
    {
        return 0;
    }

    auto sprite = Factory::CreateSprite(type);

    if (!sprite)
    {
        return 0;
    }

    try
    {
        sprite->Init(spriteData, renderContext);
    }
    catch (SpriteException& ex)
    {
        LOG_ERROR("Could not initialize user sprite: " + ex.message());
        return 0;
    }
    catch (Renderer::ShaderException& ex)
    {
        LOG_ERROR("Shader exception while initializing user sprite: " + ex.message());
        return 0;
    }
    catch (...)
    {
        LOG_ERROR("Unknown error while initializing user sprite");
        return 0;
    }

    auto spriteIdentifier = GetLowestFreeIdentifier();

    // Already at max sprites, destroy the oldest sprite to make room.
    if (m_sprites.size() == m_spriteSlots)
    {
        Destroy(m_sprites.front().first);
    }

    m_sprites.emplace_back(spriteIdentifier, std::move(sprite));
    m_spriteIdentifiers.insert(spriteIdentifier);

    return spriteIdentifier;
}

void SpriteManager::Draw(const Audio::FrameAudioData& audioData,
                         const Renderer::RenderContext& renderContext,
                         const uint32_t outputFramebufferObject,
                         const Sprite::PresetList& presets)
{
    std::vector<SpriteIdentifier> toDestroy;

    for (auto& idAndSprite : m_sprites)
    {
        idAndSprite.second->Draw(audioData, renderContext, outputFramebufferObject, presets);

        if (idAndSprite.second->Done())
        {
            toDestroy.push_back(idAndSprite.first);
        }
    }

    for (const auto id : toDestroy)
    {
        Destroy(id);
    }
}

void SpriteManager::Destroy(SpriteIdentifier spriteIdentifier)
{
    if (m_spriteIdentifiers.find(spriteIdentifier) == m_spriteIdentifiers.end())
    {
        return;
    }

    m_spriteIdentifiers.erase(spriteIdentifier);
    m_sprites.remove_if([spriteIdentifier](const auto& idAndSprite) {
        return idAndSprite.first == spriteIdentifier;
    });
}

void SpriteManager::DestroyAll()
{
    m_spriteIdentifiers.clear();
    m_sprites.clear();
}

auto SpriteManager::ActiveSpriteCount() const -> uint32_t
{
    return m_sprites.size();
}

auto SpriteManager::ActiveSpriteIdentifiers() const -> std::vector<SpriteIdentifier>
{
    std::vector<SpriteIdentifier> identifierList;
    for (auto& idAndSprite : m_sprites)
    {
        identifierList.emplace_back(idAndSprite.first);
    }

    return identifierList;
}

void SpriteManager::SpriteSlots(const uint32_t slots)
{
    m_spriteSlots = slots;

    // Remove excess sprites if limit was lowered
    while (m_sprites.size() > slots)
    {
        m_spriteIdentifiers.erase(m_sprites.front().first);
        m_sprites.pop_front();
    }
}

auto SpriteManager::SpriteSlots() const -> uint32_t
{
    return m_spriteSlots;
}

auto SpriteManager::GetSpriteVariableValue(const SpriteIdentifier spriteIdentifier,
                                           const std::string& variableName) const -> double
{
    if (m_spriteIdentifiers.find(spriteIdentifier) == m_spriteIdentifiers.end())
    {
        return 0.0;
    }

    for (const auto& idAndSprite : m_sprites)
    {
        if (idAndSprite.first == spriteIdentifier)
        {
            return idAndSprite.second->GetVariableValue(variableName);
        }
    }

    return 0.0;
}

void SpriteManager::SetSpriteVariableValue(const SpriteIdentifier spriteIdentifier,
                                           const std::string& variableName,
                                           const double value)
{
    if (m_spriteIdentifiers.find(spriteIdentifier) == m_spriteIdentifiers.end())
    {
        return;
    }

    for (const auto& idAndSprite : m_sprites)
    {
        if (idAndSprite.first == spriteIdentifier)
        {
            idAndSprite.second->SetVariableValue(variableName, value);
        }
    }
}

auto SpriteManager::GetLowestFreeIdentifier() const -> SpriteIdentifier
{
    SpriteIdentifier lowestId = 0;

    for (const auto& spriteId : m_spriteIdentifiers)
    {
        if (spriteId > lowestId + 1)
        {
            return lowestId + 1;
        }

        lowestId = spriteId;
    }

    return lowestId + 1;
}

} // namespace UserSprites
} // namespace libprojectM
