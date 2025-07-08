#pragma once

#include <string>
#include <unordered_map>
#include <optional>
#include <SDL3/SDL_opengl.h>

#include <Math/Vector.hpp>
#include <Util/Log.hpp>

namespace Renderer {
    struct TextureData {
        GLuint id = 0;
        Math::Vector2f size{ 0.0f, 0.0f };
    };

    class TextureManager {
    private:
        std::unordered_map<std::string, TextureData> m_nameToTextureData;
        std::unordered_map<GLuint, std::string> m_textureToName;
        Util::Logger m_logger;

        void DeleteTextureInternal(const std::string& name, GLuint textureID);

    public:
        TextureManager();
        ~TextureManager();
        TextureManager(const TextureManager&) = delete;
        TextureManager& operator=(const TextureManager&) = delete;
        TextureManager(TextureManager&&) noexcept;
        TextureManager& operator=(TextureManager&&) noexcept;

        TextureData* AddTexture(const std::string& name, GLuint textureID, const Math::Vector2f& size);
        TextureData* AddTextureFromFile(const std::string& name, const std::string& filePath);

        void RemoveTextureByName(const std::string& name);
        void RemoveTextureByID(GLuint textureID);

        GLuint FindTextureByName(const std::string& name) const;
        std::optional<std::string> FindNameByTextureID(GLuint textureID) const;
        std::optional<Math::Vector2f> FindSizeByName(const std::string& name) const;
        std::optional<Math::Vector2f> FindSizeByID(GLuint textureID) const;

        void Clear();
    };
}
