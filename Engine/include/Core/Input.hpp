#pragma once

#include <SDL3/SDL.h>
#include <Math/Vector.hpp>
#include <cstdint>

namespace Core {
    namespace Input {
        // Mouse input
        Math::Vector2f GetMousePosition();
        bool IsButtonDown(uint8_t button);

        // Keyboard input
        SDL_Scancode GetKeyPressed();
        bool IsKeyDown(SDL_Scancode key);
    }
}
