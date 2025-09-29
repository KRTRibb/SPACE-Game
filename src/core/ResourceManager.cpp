#include "core/ResourceManager.hpp"
#include "core/config.h"

void ResourceManager::loadTexture(const std::string& name, const std::string& filepath) {
    if (textures.count(name) == 0) {
        textures[name] = LoadTexture(filepath.c_str());
    }
}

Texture2D& ResourceManager::GetTexture(const std::string& name) {
    if (textures.count(name) == 0) {
        throw std::runtime_error("Texture not loaded: " + name);
    }

    return textures[name];
}

void ResourceManager::loadSound(const std::string& name, const std::string& filepath) {
    if (sounds.count(name) == 0) {
        sounds[name] = LoadSound(filepath.c_str());
    }
}

Sound& ResourceManager::GetSound(const std::string& name) {
    if (sounds.count(name) == 0) {
        throw std::runtime_error("Sound not loaded: " + name);
    }

    return sounds[name];
}

void ResourceManager::UnloadAll() {
    for (auto& [name, text] : textures) {
        UnloadTexture(text);
    }

    textures.clear();

    for (auto& [name, snd] : sounds) {
        UnloadSound(snd);
    }

    sounds.clear();
}

ResourceManager::~ResourceManager() {
    UnloadAll();
}
