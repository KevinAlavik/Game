#include <Core/Input.hpp>
#include <SDL3/SDL.h>
#include <Game.hpp>
#include <unordered_set>

static std::unordered_set<SDL_Scancode> keysHeld;

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
            return keysHeld.contains(key);
        }

        void ProcessEvent(const SDL_Event& event) {
            if (event.type == SDL_EVENT_KEY_DOWN) {
                keysHeld.insert(event.key.scancode);
            }
            else if (event.type == SDL_EVENT_KEY_UP) {
                keysHeld.erase(event.key.scancode);
            }
        }
    }
}
