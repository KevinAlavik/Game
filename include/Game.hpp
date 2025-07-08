#pragma once

#include <Renderer/Renderer.hpp>
#include <Renderer/TextureManager.hpp>
#include <Math/Vector.hpp>
#include <SDL3/SDL.h>

namespace Game {
    extern Renderer::Window* window;
    extern SDL_Window* rawWindow;
    extern Renderer::TextureManager* textureManager;
    extern Math::Vector2f screenSize;
}
