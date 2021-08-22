#pragma once
#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H
#include <map>
#include <string>
#include <SFML/Graphics.hpp>

class TextureManager 
{
protected:
    static TextureManager* m_s_Instance;
    std::map<std::string, sf::Texture> m_Textures;
public:
    TextureManager();
    static sf::Texture& GetTexture(std::string const &filename);
};

typedef TextureManager TheTextureManager;

#endif