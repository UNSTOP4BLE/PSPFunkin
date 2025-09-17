#include "renderergl.hpp"
#include <cassert>

namespace GFX {

void GLRenderer::init() {
#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Use compatibility profile for fixed-function pipeline
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
#endif

    assert(glfwInit());

    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL App", nullptr, nullptr);
    assert(window);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync
    glfwShowWindow(window);

    assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set top-left origin for 2D coordinates
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0.f, 1.f, 0.f, 1.f);
}

bool GLRenderer::running() {
    return !glfwWindowShouldClose(window);
}

void GLRenderer::beginFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLRenderer::endFrame() {
    glfwSwapBuffers(window);
    glfwPollEvents();
}

static void setColor(uint32_t col) {
    float r = ((col >> 16) & 0xFF) / 255.f;
    float g = ((col >> 8) & 0xFF) / 255.f;
    float b = (col & 0xFF) / 255.f;
    glColor3f(r, g, b);
}

void GLRenderer::drawRect(RECT<int32_t> rect, int z, uint32_t col) {
    setColor(col);
    glBegin(GL_QUADS);
        glVertex3i(rect.x, rect.y, z);
        glVertex3i(rect.x + rect.w, rect.y, z);
        glVertex3i(rect.x + rect.w, rect.y + rect.h, z);
        glVertex3i(rect.x, rect.y + rect.h, z);
    glEnd();
}

void GLRenderer::drawTexRect(const Texture &tex, XY<int32_t> pos, int z, uint32_t col) {
    setColor(col);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex.id);

    // Flip V coordinate because OpenGL origin is bottom-left
    glBegin(GL_QUADS);
        glTexCoord2f(0.f, 1.f); glVertex3i(pos.x, pos.y, z);                   // top-left
        glTexCoord2f(1.f, 1.f); glVertex3i(pos.x + tex.width, pos.y, z);       // top-right
        glTexCoord2f(1.f, 0.f); glVertex3i(pos.x + tex.width, pos.y + tex.height, z); // bottom-right
        glTexCoord2f(0.f, 0.f); glVertex3i(pos.x, pos.y + tex.height, z);      // bottom-left
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

}