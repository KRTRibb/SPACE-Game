#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include "raylib.h"
#include <unordered_map>
#include <string>
#include <stdexcept>

class ResourceManager {
    public:
        ResourceManager() = default;
        ~ResourceManager();

        // Textures
        void loadTexture(const std::string& name, const std::string& filepath);
        Texture2D& GetTexture(const std::string& name);

        // Sounds
        void loadSound(const std::string&name, const std::string& filepath);
        Sound& GetSound(const std::string& name);

        void UnloadAll();

    private:
        std::unordered_map<std::string, Texture2D> textures;
        std::unordered_map<std::string, Sound> sounds;

};

#endif