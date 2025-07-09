#include <Renderer/TextureManager.hpp>
#include <SDL3/SDL_opengl.h>
#include <vector>
#include <stdexcept>
#include <SDL3/SDL.h>
#if _WIN32
#include <SDL3/SDL_image.h>
#else
#include <SDL3_image/SDL_image.h>
#endif
#include <Core/Exceptions.hpp>

using namespace Renderer;

TextureManager::TextureManager()
    : m_logger("TextureManager") {
    m_logger.Debug("TextureManager created");
}

TextureManager::~TextureManager() {
    try {
        Clear();
    }
    catch (const std::exception& e) {
        m_logger.Error("Exception during destruction: {}", e.what());
    }
    m_logger.Debug("TextureManager destroyed");
}

TextureManager::TextureManager(TextureManager&& other) noexcept
    : m_nameToTextureData(std::move(other.m_nameToTextureData)),
    m_textureToName(std::move(other.m_textureToName)),
    m_logger("TextureManager") {
    other.m_nameToTextureData.clear();
    other.m_textureToName.clear();
    m_logger.Debug("TextureManager moved");
}

TextureManager& TextureManager::operator=(TextureManager&& other) noexcept {
    if (this != &other) {
        try {
            Clear();
        }
        catch (const std::exception& e) {
            m_logger.Error("Exception during move assignment Clear: {}", e.what());
        }

        m_nameToTextureData = std::move(other.m_nameToTextureData);
        m_textureToName = std::move(other.m_textureToName);

        other.m_nameToTextureData.clear();
        other.m_textureToName.clear();

        m_logger.Debug("TextureManager move-assigned");
    }
    return *this;
}

void TextureManager::DeleteTextureInternal(const std::string& name, GLuint textureID) {
    if (textureID != 0) {
        glDeleteTextures(1, &textureID);
        m_logger.Info("Deleted texture '{}' (ID {})", name, textureID);
    }
    m_nameToTextureData.erase(name);
    m_textureToName.erase(textureID);
}

TextureData* TextureManager::AddTexture(const std::string& name, GLuint textureID, const Math::Vector2f& size) {
    if (textureID == 0) {
        m_logger.Error("Attempted to add invalid texture ID 0 for '{}'", name);
        throw Core::Exception("TextureManager::AddTexture: texture ID is 0");
    }

    auto it = m_nameToTextureData.find(name);
    if (it != m_nameToTextureData.end()) {
        GLuint oldID = it->second.id;
        if (oldID != textureID) {
            DeleteTextureInternal(name, oldID);
        }
    }

    m_nameToTextureData[name] = { textureID, size };
    m_textureToName[textureID] = name;
    m_logger.Debug("Added texture '{}' (ID {}) with size {}x{}", name, textureID, size.x, size.y);
    return &m_nameToTextureData[name];
}

void TextureManager::RemoveTextureByName(const std::string& name) {
    auto it = m_nameToTextureData.find(name);
    if (it != m_nameToTextureData.end()) {
        GLuint texID = it->second.id;
        DeleteTextureInternal(name, texID);
    }
    else {
        m_logger.Error("Attempted to remove unknown texture '{}'", name);
        throw Core::Exception("TextureManager::RemoveTextureByName: texture not found: " + name);
    }
}

void TextureManager::RemoveTextureByID(GLuint textureID) {
    auto it = m_textureToName.find(textureID);
    if (it != m_textureToName.end()) {
        DeleteTextureInternal(it->second, textureID);
    }
    else {
        m_logger.Error("Attempted to remove unknown texture ID {}", textureID);
        throw Core::Exception("TextureManager::RemoveTextureByID: texture ID not found");
    }
}

GLuint TextureManager::FindTextureByName(const std::string& name) const {
    auto it = m_nameToTextureData.find(name);
    if (it != m_nameToTextureData.end()) {
        return it->second.id;
    }
    throw Core::Exception("TextureManager::FindTextureByName: texture not found: " + name);
}

std::optional<std::string> TextureManager::FindNameByTextureID(GLuint textureID) const {
    auto it = m_textureToName.find(textureID);
    if (it != m_textureToName.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<Math::Vector2f> TextureManager::FindSizeByName(const std::string& name) const {
    auto it = m_nameToTextureData.find(name);
    if (it != m_nameToTextureData.end()) {
        return it->second.size;
    }
    return std::nullopt;
}

std::optional<Math::Vector2f> TextureManager::FindSizeByID(GLuint textureID) const {
    auto nameOpt = FindNameByTextureID(textureID);
    if (!nameOpt.has_value())
        return std::nullopt;
    return FindSizeByName(nameOpt.value());
}

void TextureManager::Clear() {
    if (m_nameToTextureData.empty()) {
        m_logger.Debug("No textures to clear");
        return;
    }

    std::vector<GLuint> texIDs;
    texIDs.reserve(m_nameToTextureData.size());
    for (const auto& pair : m_nameToTextureData) {
        texIDs.push_back(pair.second.id);
    }

    glDeleteTextures(static_cast<GLsizei>(texIDs.size()), texIDs.data());
    m_logger.Info("Deleted {} textures", texIDs.size());

    m_nameToTextureData.clear();
    m_textureToName.clear();
}

TextureData* TextureManager::AddTextureFromFile(const std::string& name, const std::string& filePath) {
    SDL_Surface* surface = IMG_Load(filePath.c_str());
    if (!surface) {
        std::string errorMsg = "Failed to load image '" + filePath + "': " + SDL_GetError();
        m_logger.Error("{}", errorMsg);
        throw Core::Exception(errorMsg);
    }

    SDL_PixelFormat format = SDL_PIXELFORMAT_ABGR8888;
    SDL_Surface* converted = SDL_ConvertSurface(surface, format);
    SDL_DestroySurface(surface);

    if (!converted) {
        std::string errorMsg = "Failed to convert surface for '" + filePath + "': " + SDL_GetError();
        m_logger.Error("{}", errorMsg);
        throw Core::Exception(errorMsg);
    }

    int width = converted->w;
    int height = converted->h;
    Uint32* pixels = static_cast<Uint32*>(converted->pixels);
    std::vector<Uint32> flipped(width * height);

    for (int y = 0; y < height; ++y) {
        memcpy(&flipped[y * width],
            &pixels[(height - 1 - y) * (converted->pitch / 4)],
            width * sizeof(Uint32));
    }

    GLuint textureID = 0;
    glGenTextures(1, &textureID);
    if (textureID == 0) {
        SDL_DestroySurface(converted);
        std::string errorMsg = "Failed to generate OpenGL texture for '" + filePath + "'";
        m_logger.Error("{}", errorMsg);
        throw Core::Exception(errorMsg);
    }

    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, flipped.data());

    SDL_DestroySurface(converted);

    TextureData* tex = AddTexture(name, textureID, Math::Vector2f(static_cast<float>(width), static_cast<float>(height)));
    m_logger.Info("Loaded texture '{}' from '{}' (ID {}) size {}x{}", name, filePath, textureID, width, height);
    return tex;
}
