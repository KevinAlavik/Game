#include <Renderer/Window.hpp>
#include <SDL3/SDL_opengl.h>

using namespace Renderer;

/* ============================================================== */
/* Low-level window functions                                    */
/* ============================================================== */
Window::Window(const std::string& title, Math::Vector2f size)
    : m_title(title), m_window(nullptr), m_logger("Window") {

    m_logger.Info("Creating window: {} ({}, {})", title, static_cast<int>(size.x), static_cast<int>(size.y));

    // Create SDL window
    m_window = SDL_CreateWindow(title.c_str(), static_cast<int>(size.x), static_cast<int>(size.y), SDL_WINDOW_OPENGL);
    if (!m_window) {
        std::string errorMsg = SDL_GetError();
        m_logger.Error("Failed to create window: {}", errorMsg);
        throw Core::WindowException("Failed to create SDL Window: " + errorMsg);
    }
    else {
        m_logger.Debug("Window created successfully");
    }

    // Create OpenGL context
    m_glcontext = SDL_GL_CreateContext(m_window);
    if (!m_glcontext) {
        std::string errorMsg = SDL_GetError();
        m_logger.Error("Failed to create OpenGL context: {}", errorMsg);
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
        throw Core::WindowException("Failed to create OpenGL Context: " + errorMsg);
    }
    else {
        m_logger.Debug("OpenGL context created successfully");
    }
    UpdateFPS();
}

Window::~Window() {
    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_logger.Warn("Window destroyed: {}", m_title);
    }
    else {
        m_logger.Warn("Attempted to destroy a null window");
    }
}

SDL_Window* Window::GetRawWindow() const {
    return m_window;
}

SDL_GLContext Window::GetGLContext() const {
    return m_glcontext; 
}

void Window::Poll() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        m_lastEvent = event;
    }
}

SDL_Event Window::GetLastEvent() const {
    return m_lastEvent;
}

/* ============================================================== */
/* High-level window API                                          */
/* ============================================================== */
bool Window::ShouldExit() {
    return m_lastEvent.type == SDL_EVENT_QUIT;
}

bool Window::SetTitle(const std::string& title) {
    if (m_window) {
        SDL_SetWindowTitle(m_window, title.c_str());
        return true;
    }
    else {
        m_logger.Error("Cannot set title, window is null");
        return false;
    }
}

void Window::UpdateFPS() {
    uint64_t now = SDL_GetTicks();
    if (m_lastTime == 0) {
        m_lastTime = now;
        m_prevFrameTime = now;
        m_deltaTime = 0.0f;
        return;
    }

    m_deltaTime = (now - m_prevFrameTime) / 1000.0f;
    m_prevFrameTime = now;

    m_frameCount++;
    uint64_t elapsed = now - m_lastTime;
    if (elapsed >= 1000) {
        m_fps = m_frameCount * 1000.0f / elapsed;
        m_frameCount = 0;
        m_lastTime = now;
    }
}


float Window::GetFPS() const {
    return m_fps;
}

Math::Vector2f Window::GetMousePosition() const {
    float x = 0, y = 0;
    SDL_GetMouseState(&x, &y);
    return Math::Vector2f(x, y);
}

bool Window::IsButtonDown(uint8_t button) const {
    uint32_t buttons = SDL_GetMouseState(nullptr, nullptr);
    return (buttons & SDL_BUTTON_MASK(button)) != 0;
}

float Window::GetDeltaTime() const {
    return m_deltaTime;
}
