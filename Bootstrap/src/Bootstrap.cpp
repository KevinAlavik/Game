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
#include "LuaManager.hpp"
#include "game_lua.hpp"

namespace Game {
    Util::Logger logger("MainGame");
    Renderer::Window* window = nullptr;
    SDL_Window* rawWindow = nullptr;
    Renderer::TextureManager* textureManager = nullptr;
    Math::Vector2f screenSize(900.0f, 700.0f);
    LuaManager lua;
}

using namespace Game;

void RegisterLuaAPI() {
    // Engine functions
    lua.RegisterTable("engine")
        .RegisterFunction("exit", []() {
            logger.Info("Exiting game...");
            std::exit(0);
        });

    // Window functions
    lua.RegisterTable("window")
        .RegisterFunction("setTitle", [](const std::string& title) { window->SetTitle(title); })
        .RegisterFunction("getFPS", []() { return window->GetFPS(); })
        .RegisterFunction("getMousePos", []() { return window->GetMousePosition(); })
        .RegisterFunction("isButtonDown", [](uint8_t button) { return window->IsButtonDown(button); })
        .RegisterVariable("width", screenSize.x)
        .RegisterVariable("height", screenSize.y);

    // Draw functions
    lua.RegisterTable("draw")
        .RegisterFunction("init", [](float w, float h, bool vsync) { Renderer::Draw::Init({w,h}, vsync); })
        .RegisterFunction("clear", [](float r, float g, float b, float a) { Renderer::Draw::Clear({r,g,b,a}); })
        .RegisterFunction("texturedQuad", [](GLuint texID, Math::Vector2f size, Math::Vector2f pos) {
            Renderer::Draw::TexturedQuad(texID, size, pos);
        });

    // Texture manager functions
    lua.RegisterTable("texture")
        .RegisterFunction("load", [](const std::string& name, const std::string& path) -> sol::object {
            auto tex = textureManager->AddTextureFromFile(name, path);
            if (!tex) return sol::nil;

            sol::table t = lua.CreateRawTable();
            t["id"]     = tex->id;
            t["width"]  = tex->size.x;
            t["height"] = tex->size.y;
            return t;
        });

    // Logger
    lua.RegisterTable("logger")
        .RegisterFunction("new", [](const std::string& name) -> sol::object {
            logger.Debug("Creating new logger with name {}", name);

            auto l = std::make_shared<Util::Logger>(name);

            sol::table t = lua.CreateRawTable();
            t["__self"] = l;

            // Logger methods
            t["info"]  = [](sol::table self, const std::string& msg) {
                auto l_ptr = self["__self"].get<std::shared_ptr<Util::Logger>>();
                l_ptr->Info("{}", msg);
            };
            t["warn"]  = [](sol::table self, const std::string& msg) {
                auto l_ptr = self["__self"].get<std::shared_ptr<Util::Logger>>();
                l_ptr->Warn("{}", msg);
            };
            t["error"] = [](sol::table self, const std::string& msg) {
                auto l_ptr = self["__self"].get<std::shared_ptr<Util::Logger>>();
                l_ptr->Error("{}", msg);
            };
            t["debug"] = [](sol::table self, const std::string& msg) {
                auto l_ptr = self["__self"].get<std::shared_ptr<Util::Logger>>();
                l_ptr->Debug("{}", msg);
            };

            sol::table mt = lua.CreateRawTable();
            mt["__index"] = t;
            t[sol::metatable_key] = mt;

            return sol::make_object(lua.GetState().lua_state(), t);
        });

    // Vector2f
    lua.GetState().new_usertype<Math::Vector2f>(
        "Vector2f",
        sol::constructors<Math::Vector2f(), Math::Vector2f(float, float)>(),
        "x", &Math::Vector2f::x,
        "y", &Math::Vector2f::y,
        "add", [](const Math::Vector2f& a, const Math::Vector2f& b) { return a + b; },
        "sub", [](const Math::Vector2f& a, const Math::Vector2f& b) { return a - b; },
        "mul", [](const Math::Vector2f& v, float s) { return v * s; },
        "eq", [](const Math::Vector2f& a, const Math::Vector2f& b) { return a == b; }
    );

    // Vector3f
    lua.GetState().new_usertype<Math::Vector3f>(
        "Vector3f",
        sol::constructors<Math::Vector3f(), Math::Vector3f(float, float, float)>(),
        "x", &Math::Vector3f::x,
        "y", &Math::Vector3f::y,
        "z", &Math::Vector3f::z,
        "add", [](const Math::Vector3f& a, const Math::Vector3f& b) { return a + b; },
        "sub", [](const Math::Vector3f& a, const Math::Vector3f& b) { return a - b; },
        "mul", [](const Math::Vector3f& v, float s) { return v * s; },
        "eq", [](const Math::Vector3f& a, const Math::Vector3f& b) { return a == b; }
    );

    // Vector4f
    lua.GetState().new_usertype<Math::Vector4f>(
        "Vector4f",
        sol::constructors<Math::Vector4f(), Math::Vector4f(float, float, float, float)>(),
        "x", &Math::Vector4f::x,
        "y", &Math::Vector4f::y,
        "z", &Math::Vector4f::z,
        "w", &Math::Vector4f::w,
        "add", [](const Math::Vector4f& a, const Math::Vector4f& b) { return a + b; },
        "sub", [](const Math::Vector4f& a, const Math::Vector4f& b) { return a - b; },
        "mul", [](const Math::Vector4f& v, float s) { return v * s; },
        "eq", [](const Math::Vector4f& a, const Math::Vector4f& b) { return a == b; }
    );
}

namespace {
    bool Initialize() {
        Util::Logger::SetLogLevel(Util::Logger::Level::Debug);

        if (!Renderer::InitSDL()) {
            logger.Error("Failed to initialize SDL");
            return false;
        }

        logger.Debug("SDL and SDL_image initialized successfully");

        window = new Renderer::Window("Game", screenSize);
        rawWindow = window->GetRawWindow();
        Renderer::Draw::Init(screenSize, false);

        textureManager = new Renderer::TextureManager();

        RegisterLuaAPI();
        if (!lua.LoadScriptFromMemory(GameLuaCode)) {
            logger.Error("Failed to load embedded Lua script");
            return false;
        }

        lua.Call("setup");

        return true;
    }

    void Cleanup() {
        delete textureManager;
        textureManager = nullptr;

        delete window;
        window = nullptr;

        SDL_Quit();
    }

    void MainLoop() {
        while (!window->ShouldExit()) {
            window->Poll();
            window->UpdateFPS();
            float deltaTime = window->GetDeltaTime();

            lua.Call("update", deltaTime);
            lua.Call("render");

            Renderer::Render(window);
        }
    }
}

int main(int argc, char* argv[]) {
    try {
        if (!Initialize()) return -1;
        MainLoop();
        Cleanup();
    } catch (const Core::WindowException& e) {
        logger.Error("Window error: {}", e.what());
        Cleanup();
        return -1;
    } catch (const Core::Exception& e) {
        logger.Error("Core exception: {}", e.what());
        Cleanup();
        return -1;
    } catch (const std::exception& e) {
        logger.Error("Standard exception: {}", e.what());
        Cleanup();
        return -1;
    } catch (...) {
        logger.Error("Unknown exception occurred");
        Cleanup();
        return -1;
    }

    return 0;
}
