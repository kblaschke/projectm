#ifndef TextureManager_HPP
#define TextureManager_HPP

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "projectM-opengl.h"
#include "Texture.hpp"
#include "FileScanner.hpp"


class TextureManager
{
  std::string presetsURL;
  std::map<std::string, Texture*> textures;
  std::vector<Texture*> blurTextures;
  Texture * mainTexture;

  std::vector<std::string> random_textures;
  TextureSamplerDesc loadTexture(const std::string name, const std::string imageUrl);
  void ExtractTextureSettings(const std::string qualifiedName, GLint &_wrap_mode, GLint &_filter_mode, std::string & name);
  std::vector<std::string> extensions;

public:
  TextureManager(std::string _presetsURL, const int texsizeX, const int texsizeY,
                 std::string datadir = "");
  ~TextureManager();

  void Clear();
  void Preload();

  /**
   * @brief Adds or replaces the given texture with the new OpenGL texture.
   * @param name The internal name of the texture.
   * @param textureId The OpenGL texture to add.
   * @param width The width of the texture.
   * @param height The height of the texture.
   * @return A descriptor that describes the newly added texture.
   */
  TextureSamplerDesc addTexture(const std::string& name, GLuint textureId, size_t width, size_t height);

  TextureSamplerDesc tryLoadingTexture(const std::string name);
  TextureSamplerDesc getTexture(const std::string fullName, const GLenum defaultWrap, const GLenum defaultFilter);
  const Texture * getMainTexture() const;
  const std::vector<Texture *> & getBlurTextures() const;

  void updateMainTexture();

  TextureSamplerDesc getRandomTextureName(std::string rand_name);
  void clearRandomTextures();
};

#endif
