#include <iostream>
#include <Renderer/Renderer.hpp>
#include <Renderer/Draw.hpp>
#include <Renderer/TextureManager.hpp>
#include <Util/Log.hpp>
#include <Core/Exceptions.hpp>
#include <SDL3/SDL_opengl.h>
#include <Math/Vector.hpp>
#include <SDL3/SDL.h>
#include <Core/Input.hpp>

// ImGui includes
#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_opengl3.h>

namespace Game {
    Util::Logger logger("Main");
    Renderer::Window* window = nullptr;
    SDL_Window* rawWindow = nullptr;
    Renderer::TextureManager* textureManager = nullptr;
    Math::Vector2f screenSize(900.0f, 700.0f);
    Renderer::TextureData* shrekTexture = nullptr;
}

using namespace Game;

namespace {
    bool Initialize() {
#ifdef DEBUG_BUILD
        Util::Logger::SetLogLevel(Util::Logger::Level::Debug);
#else
        Util::Logger::SetLogLevel(Util::Logger::Level::Info);
#endif

        if (!Renderer::InitSDL()) {
            logger.Error("Failed to initialize SDL");
            return false;
        }

        logger.Debug("SDL and SDL_image initialized successfully");

        window = new Renderer::Window("Game", screenSize);
        rawWindow = window->GetRawWindow();
        Renderer::Draw::Init(screenSize, false);

        textureManager = new Renderer::TextureManager();
        shrekTexture = textureManager->AddTextureFromFile("shrek", "assets/shrek.png");

        // ImGui initialization
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImGui::StyleColorsDark();

        ImGui_ImplSDL3_InitForOpenGL(rawWindow, window->GetGLContext());
        ImGui_ImplOpenGL3_Init("#version 330 core");

        return true;
    }

    void Cleanup() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();

        delete textureManager;
        textureManager = nullptr;

        delete window;
        window = nullptr;

        SDL_Quit();
    }

    /* ============================================================== */
    /* MAIN GAME LOGIC                                                */
    /* ============================================================== */
    bool IsMouseOnTexture(const Math::Vector2f& mousePos, const Math::Vector2f& texPos, const Math::Vector2f& texSize) {
        float halfWidth = texSize.x / 2.0f;
        float halfHeight = texSize.y / 2.0f;

        return mousePos.x >= (texPos.x - halfWidth) &&
            mousePos.x <= (texPos.x + halfWidth) &&
            mousePos.y >= (texPos.y - halfHeight) &&
            mousePos.y <= (texPos.y + halfHeight);
    }

    void RenderImGui() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        bool show_window = false;
        ImGui::Begin("Controls", &show_window, ImGuiWindowFlags_NoResize);
        ImGui::Text("Use WASD to move the texture");
        ImGui::Text("Left click and drag to move the texture around");
        ImGui::Text("Press ESC to exit");
        ImGui::End();
	}

    void MainLoop() {
        Math::Vector2f mousePos(0.0f, 0.0f);

        bool dragging = false;
        Math::Vector2f dragOffset(0.0f, 0.0f);
        Math::Vector2f texturePos(Game::screenSize.x / 2, Game::screenSize.y / 2);
        bool wasLeftButtonDown = false;

        const float moveSpeed = 300.0f; // pixels per second

        while (!Game::window->ShouldExit()) {
            // Update FPS
            Game::window->UpdateFPS();
            Game::window->SetTitle("Game - FPS: " + std::to_string(Game::window->GetFPS()));
            float deltaTime = Game::window->GetDeltaTime();

            // Render ImGui frame
            RenderImGui();

            // Handle mouse events
            mousePos = Core::Input::GetMousePosition();
            bool isLeftDown = Core::Input::IsButtonDown(SDL_BUTTON_LEFT);
            if (isLeftDown && !wasLeftButtonDown) {
                if (IsMouseOnTexture(mousePos, texturePos, Game::shrekTexture->size)) {
                    dragging = true;
                    dragOffset = mousePos - texturePos;
                }
            }
            if (!isLeftDown && wasLeftButtonDown) {
                dragging = false;
            }

            wasLeftButtonDown = isLeftDown;

            if (dragging) {
                texturePos = mousePos - dragOffset;
            }

            // WASD keyboard movement
            if (Core::Input::IsKeyDown(SDL_SCANCODE_W)) {
                texturePos.y -= moveSpeed * deltaTime;
            }
            if (Core::Input::IsKeyDown(SDL_SCANCODE_S)) {
                texturePos.y += moveSpeed * deltaTime;
            }
            if (Core::Input::IsKeyDown(SDL_SCANCODE_A)) {
                texturePos.x -= moveSpeed * deltaTime;
            }
            if (Core::Input::IsKeyDown(SDL_SCANCODE_D)) {
                texturePos.x += moveSpeed * deltaTime;
            }

            // Handle ESC key to exit
            if (Core::Input::IsKeyDown(SDL_SCANCODE_ESCAPE)) {
                Game::window->Poll();
                break;
            }

            // Clamp texture position to screen bounds
            float halfWidth = Game::shrekTexture->size.x / 2.0f;
            float halfHeight = Game::shrekTexture->size.y / 2.0f;
            if (texturePos.x < halfWidth) {
                texturePos.x = halfWidth;
            }
            if (texturePos.x > Game::screenSize.x - halfWidth) {
                texturePos.x = Game::screenSize.x - halfWidth;
            }
            if (texturePos.y < halfHeight) {
                texturePos.y = halfHeight;
            }
            if (texturePos.y > Game::screenSize.y - halfHeight) {
                texturePos.y = Game::screenSize.y - halfHeight;
            }

            // Render
            Renderer::Draw::Clear(Math::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
            Renderer::Draw::TexturedQuad(Game::shrekTexture->id, Game::shrekTexture->size, texturePos);

            // ImGui render
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            
			// Finish the rendering
            Renderer::Render(Game::window);

            // Poll events
            Game::window->Poll();

            // Feed events to ImGui and your input system
            SDL_Event e = Game::window->GetLastEvent();
            ImGui_ImplSDL3_ProcessEvent(&e);
            Core::Input::ProcessEvent(e);
        }
    }
}

int main(int argc, char* argv[]) {
    try {
        if (!Initialize()) {
            return -1;
        }

        logger.Info("--- My Epic Game ---");
        logger.Info("Use: WASD to move the texture");
        logger.Info("Left click and drag to move the texture around");
        logger.Info("Press ESC to exit");
        logger.Info("--------------------");
        MainLoop();
        Cleanup();
    }
    catch (const Core::WindowException& e) {
        logger.Error("Window error: {}", e.what());
        Cleanup();
        return -1;
    }
    catch (const Core::Exception& e) {
        logger.Error("Core exception: {}", e.what());
        Cleanup();
        return -1;
    }
    catch (const std::exception& e) {
        logger.Error("Standard exception: {}", e.what());
        Cleanup();
        return -1;
    }
    catch (...) {
        logger.Error("Unknown exception occurred");
        Cleanup();
        return -1;
    }

    return 0;
}
