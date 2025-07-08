#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <Util/Log.hpp>
#include <Core/Exceptions.hpp>
#include <Math/Vector.hpp>

namespace Renderer {
    class Window {
    private:
        std::string m_title;
        SDL_Window* m_window;
        SDL_GLContext m_glcontext;
        SDL_Event m_lastEvent;

        // Utils
        Util::Logger m_logger;

        // Frame rate tracking
        uint64_t m_lastTime = 0;
        uint32_t m_frameCount = 0;
        float m_fps = 0.0f;

        // Timing
        uint64_t m_prevFrameTime = 0;
        float m_deltaTime = 0.0f;
    public:
        // Low-level window functions
        Window(const std::string& title, Math::Vector2f size);
        ~Window();
        SDL_Window* GetRawWindow();
        void Poll();
        SDL_Event GetLastEvent() const;
    public:
        // High-level window API
        bool ShouldExit();
        bool SetTitle(const std::string& title);

        void UpdateFPS();
        float GetFPS() const;

        Math::Vector2f GetMousePosition() const;
        bool IsButtonDown(uint8_t button) const;

        float GetDeltaTime() const;
    };
}
