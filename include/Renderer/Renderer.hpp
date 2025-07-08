#pragma once

#include <Renderer/Window.hpp>
#include <Util/Log.hpp>

namespace Renderer
{
	static Util::Logger logger("Renderer");
	bool InitSDL();
	void Render(Renderer::Window* window);
}