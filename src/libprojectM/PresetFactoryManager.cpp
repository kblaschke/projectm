#include "PresetFactoryManager.hpp"

#include "Utils.hpp"

#include <MilkdropPreset/Factory.hpp>

#include <Logging.hpp>

#include <cassert>
#include <iostream>
#include <sstream>

namespace libprojectM {

PresetFactoryManager::~PresetFactoryManager()
{
    ClearFactories();
}

void PresetFactoryManager::ClearFactories()
{
    m_factoryMap.clear();
    for (std::vector<PresetFactory*>::iterator pos = m_factoryList.begin();
         pos != m_factoryList.end(); ++pos)
    {
        assert(*pos);
        delete (*pos);
    }
    m_factoryList.clear();
}

void PresetFactoryManager::Initialize()
{
    ClearFactories();

    auto* milkdropFactory = new MilkdropPreset::Factory();
    RegisterFactory(milkdropFactory->supportedExtensions(), milkdropFactory);
}

void PresetFactoryManager::RegisterFactory(const std::string& extensions, PresetFactory* factory)
{
    // Current behavior if an extension conflict occurs is to override the previous once
    std::stringstream ss(extensions);
    std::string extension;

    m_factoryList.push_back(factory);

    while (ss >> extension)
    {
        if (m_factoryMap.count(extension))
        {
            LOG_ERROR("[PresetFactoryManager] Warning: extension \"" + extension + "\" already has a factory. New factory handler ignored.");
        }
        else
        {
            m_factoryMap.insert(std::make_pair(extension, factory));
        }
    }
}


std::unique_ptr<Preset> PresetFactoryManager::CreatePresetFromFile(const std::string& filename, const Renderer::RenderContext& renderContext)
{
    try
    {
        const std::string extension = "." + ParseExtension(filename);

        return Factory(extension).LoadPresetFromFile(filename, renderContext);
    }
    catch (const PresetFactoryException&)
    {
        throw;
    }
    catch (const std::exception& e)
    {
        throw PresetFactoryException(e.what());
    }
    catch (...)
    {
        throw PresetFactoryException("[PresetFactoryManager] Uncaught preset factory exception.");
    }
}

std::unique_ptr<Preset> PresetFactoryManager::CreatePresetFromStream(const std::string& extension, std::istream& data, const Renderer::RenderContext& renderContext)
{
    try
    {
        return Factory(extension).LoadPresetFromStream(data, renderContext);
    }
    catch (const PresetFactoryException&)
    {
        throw;
    }
    catch (const std::exception& e)
    {
        throw PresetFactoryException(e.what());
    }
    catch (...)
    {
        throw PresetFactoryException("[PresetFactoryManager] Uncaught preset factory exception.");
    }
}

PresetFactory& PresetFactoryManager::Factory(const std::string& extension)
{
    if (!ExtensionHandled(extension))
    {
        std::string error = "[PresetFactoryManager] No preset factory associated with extension \"";
        error += extension;
        error += "\"";
        LOG_ERROR(error);
        throw PresetFactoryException(error);
    }

    return *m_factoryMap[extension];
}

bool PresetFactoryManager::ExtensionHandled(const std::string& extension) const
{
    return m_factoryMap.count(extension);
}

std::vector<std::string> PresetFactoryManager::ExtensionsHandled() const
{
    std::vector<std::string> retval;
    for (auto const& element : m_factoryMap)
    {
        retval.push_back(element.first);
    }
    return retval;
}

//CPP17: std::filesystem::path::extension
auto PresetFactoryManager::ParseExtension(const std::string& filename) -> std::string
{
    const auto start = filename.find_last_of('.');

    if (start == std::string::npos || start >= (filename.length() - 1)) {
        return "";
    }

    return Utils::ToLower(filename.substr(start + 1, filename.length()));
}

} // namespace libprojectM
