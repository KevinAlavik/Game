#include <Renderer/Draw.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

static int g_screenWidth = 800;
static int g_screenHeight = 600;

void Renderer::Draw::Init(const Math::Vector2f& screenSize, bool vsync) {
    g_screenWidth = static_cast<int>(screenSize.x);
    g_screenHeight = static_cast<int>(screenSize.y);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, g_screenWidth, g_screenHeight, 0, -1, 1); // Top-left origin

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    SDL_GL_SetSwapInterval(vsync ? 1 : 0);
}

void Renderer::Draw::Clear(Math::Vector4f color) {
    glClearColor(color.x, color.y, color.z, color.w);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::Draw::TexturedQuad(GLuint textureID, const Math::Vector2f& size, const Math::Vector2f& pos) {
    if (textureID == 0 || size.x <= 0 || size.y <= 0) {
        SDL_Log("Draw::TexturedQuad: Invalid parameters (textureID=%u, size=%.2fx%.2f)", textureID, size.x, size.y);
        return;
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    Math::Vector2f half = size * 0.5f;

    // Quad centered at 'pos'
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(pos.x - half.x, pos.y - half.y); // bottom-left
    glTexCoord2f(1.0f, 1.0f); glVertex2f(pos.x + half.x, pos.y - half.y); // bottom-right
    glTexCoord2f(1.0f, 0.0f); glVertex2f(pos.x + half.x, pos.y + half.y); // top-right
    glTexCoord2f(0.0f, 0.0f); glVertex2f(pos.x - half.x, pos.y + half.y); // top-left
    glEnd();

    glDisable(GL_TEXTURE_2D);
}
