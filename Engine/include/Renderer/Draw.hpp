#pragma once

#include <SDL3/SDL_opengl.h>
#include <Core/Exceptions.hpp>
#include <Math/Vector.hpp>

namespace Renderer {
    namespace Draw {
        void Init(const Math::Vector2f& screenSize = Math::Vector2f(800.0f, 600.0f), bool vsync = true);
        void Clear(Math::Vector4f color);
        void TexturedQuad(GLuint textureID, const Math::Vector2f& size, const Math::Vector2f& pos);
    }
}
