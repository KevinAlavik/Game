#include <Renderer/Renderer.hpp>
#include <SDL3/SDL.h>
#include <Renderer/Window.hpp>

bool Renderer::InitSDL() {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		logger.Error("Could not initialize SDL: %s", SDL_GetError());
		return false;
	}
	return true;
}

void Renderer::Render(Renderer::Window* window) {
	SDL_GL_SwapWindow(window->GetRawWindow());
}