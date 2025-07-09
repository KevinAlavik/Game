#include <Core/Input.hpp>
#include <SDL3/SDL.h>
#include <Game.hpp>
#include <unordered_set>

namespace Core {
    namespace Input {
        // Mouse
        Math::Vector2f GetMousePosition() {
            float x = 0, y = 0;
            SDL_GetMouseState(&x, &y);
            return Math::Vector2f(x, y);
        }

        bool IsButtonDown(uint8_t button) {
            uint32_t buttons = SDL_GetMouseState(nullptr, nullptr);
            return (buttons & SDL_BUTTON_MASK(button)) != 0;
        }

        // Keyboard
        SDL_Scancode GetKeyPressed() {
            SDL_Event event = Game::window->GetLastEvent();
            if (event.type == SDL_EVENT_KEY_DOWN) {
                return event.key.scancode;
            }
            return SDL_SCANCODE_UNKNOWN;
        }

        bool IsKeyDown(SDL_Scancode key) {
            SDL_Event event = Game::window->GetLastEvent();
            if (event.type == SDL_EVENT_KEY_DOWN) {
                return event.key.scancode == key;
            }
            else if (event.type == SDL_EVENT_KEY_UP) {
                return false; // Key is not down if it's released
            }
            return false;
        }
    }
}
