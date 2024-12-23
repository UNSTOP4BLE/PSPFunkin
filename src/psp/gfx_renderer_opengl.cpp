//written by spicyjpeg and UNSTOP4BLE
#include "glad/gl.h"
#include "../main.h"
#include "file.h"
#include "gfx_renderer_opengl.h"
#include <fstream>    

#ifdef ENABLE_OPENGL_RENDERER

#include "gfx_common.h"
#include "util.h"

static unsigned int loadshader(const char* path, GLenum type) {
    std::ifstream t(path);
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string source = buffer.str();
    t.close();

    unsigned int shader = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(shader, 1, static_cast<char const * const *>(&src), NULL);
    glCompileShader(shader);

    int  success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        debugLog("ERROR::SHADER::COMPILATION_FAILED: %s", infoLog);
    }

    return shader;
}

namespace Gfx {

OpenGLTexture::OpenGLTexture(TextureFormat _format, bool _bilinearFilter, int _width, int _height) {
    // TODO
}

OpenGLTexture::~OpenGLTexture(void) {
    // TODO
}

void OpenGLTexture::loadData(const void *data) {
    // TODO
}

OpenGLRenderer::OpenGLRenderer(int width, int height) : Renderer(width, height) {
    // OpenGL 3.3 core profile
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,          1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,  SDL_GL_CONTEXT_PROFILE_CORE);

    window = SDL_CreateWindow(
        "PSPFunkin",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    glContext = SDL_GL_CreateContext(window);

    int version = gladLoadGL((GLADloadfunc) SDL_GL_GetProcAddress);
    printf("GL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

    //load shaders
    vertexShader = loadshader(getPath("assets/shaders/vertexShader.glsl").c_str(), GL_VERTEX_SHADER);
    fragmentShader = loadshader(getPath("assets/shaders/fragmentShader.glsl").c_str(), GL_FRAGMENT_SHADER);

    //set up shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    //free shaders    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);  
/*
  
 // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 

    // render loop
    // -----------
    while (1)
    {

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
 //       glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind it every time 
 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        SDL_GL_SwapWindow(window);
    }*/
}

OpenGLRenderer::~OpenGLRenderer(void) {
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
}

void OpenGLRenderer::beginFrame(void) {
    // TODO
}

void OpenGLRenderer::endFrame(void) {
    // TODO
}

void OpenGLRenderer::swapBuffers(void) {
    SDL_GL_SwapWindow(window);
}

void *OpenGLRenderer::allocatePrimData(size_t length) {
    // TODO
    return nullptr;
}

void OpenGLRenderer::setMatrix(const Mat4 &mat) {
    // TODO
}

void OpenGLRenderer::setTexture(const Texture &tex) {
    // TODO
}

void OpenGLRenderer::clear(Color color, int z) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::drawPoints(const Point *prims, size_t count) {
    // TODO
}

void OpenGLRenderer::drawLines(const Line *prims, size_t count) {

//    int flags = 0
  //      | GU_COLOR_8888    // 24bpp vertex colors
    //    | GU_VERTEX_32BITF // float vertices
      //  | GU_TRANSFORM_3D; // use matrices

//  sceGuDisable(GU_TEXTURE_2D);
    //sceGuDrawArray(GU_LINES, flags, count * 2, nullptr, prims); // 2 vertices per primitive

    glDrawArrays(GL_LINES, 0, count * 2);

    // TODO    
//    _setPrimType(PRIM_TYPE_LINES);
  //  auto p = _addPrim<Line>();

    //for (int i = 0; i < 2; i++) {
     //   p[i]->color = line[i].color;
     //   p[i]->pos   = line[i].pos;
    //}
}

void OpenGLRenderer::drawTriangles(const Triangle *prims, size_t count) {
    // TODO
}

void OpenGLRenderer::drawTexturedTriangles(const TexturedTriangle *prims, size_t count) {
    // TODO
}

}

#endif
