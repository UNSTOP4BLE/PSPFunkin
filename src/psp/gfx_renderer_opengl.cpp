//written by spicyjpeg and UNSTOP4BLE

#include "glad/gl.h"
#include "../main.h"
#include "file.h"
#include "gfx_renderer_opengl.h"
#include <fstream>    

#ifdef ENABLE_OPENGL_RENDERER

#include "gfx_common.h"
#include "util.h"

namespace Gfx {

template<typename T> void setUniformVar(unsigned int ID, const std::string& name, T value) {
    debugLog("ERROR::SHADER::SETUNIFORM: Unsupported type %s", name.c_str());
}

// Specialization for int
template<> void setUniformVar<int>(unsigned int ID, const std::string& name, int value) {
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location != -1) {
        glUniform1i(location, value); // For integer uniforms
    } else {
        debugLog("ERROR::SHADER::SETUNIFORM: Uniform not found! %s\n", name.c_str());
    }
}

// Specialization for float
template<> void setUniformVar<float>(unsigned int ID, const std::string& name, float value) {
    GLint location = glGetUniformLocation(ID, name.c_str());
    if (location != -1) {
        glUniform1f(location, value); // For float uniforms
    } else {
        debugLog("ERROR::SHADER::SETUNIFORM: Uniform not found! %s\n", name.c_str());
    }
}

void OpenGLShader::loadshader(const char* path, GLenum type) {
    std::ifstream t(path);
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string source = buffer.str();
    t.close();
    
    unsigned int builtshader = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(builtshader, 1, static_cast<char const * const *>(&src), NULL);
    glCompileShader(builtshader);
    
    int  success;
    char infoLog[512];
    glGetShaderiv(builtshader, GL_COMPILE_STATUS, &success);
    
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        debugLog("ERROR::SHADER::COMPILATION_FAILED: %s", infoLog);
    }
    shader = builtshader;
}
    
void OpenGLShader::freeshader(void) {         
    glDeleteShader(shader);
}

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
    vertexShader.loadshader(getPath("assets/shaders/vertexShader.glsl").c_str(), GL_VERTEX_SHADER);
    fragmentShader.loadshader(getPath("assets/shaders/fragmentShader.glsl").c_str(), GL_FRAGMENT_SHADER);
    //set up shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader.shader);
    glAttachShader(shaderProgram, fragmentShader.shader);
    glLinkProgram(shaderProgram);
    //free shaders    
    vertexShader.freeshader();
    fragmentShader.freeshader();  

  
 // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    // render loop
    // -----------
    while (1)
    {

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        Color r;
        r.setValue(255,0,0,255);
        Color g;
        g.setValue(0,255,0,255);
        Color b;
        b.setValue(0,0,255,255);

        Gfx::Triangle triangle = {
            {r, width/2, height/4, 0},
            {g, width/4, (height/4)+(height/2), 0}, 
            {b, (width/4)+(width/2), (height/4)+(height/2), 0}
        };   

        drawTriangles(&triangle, 1);

        SDL_GL_SwapWindow(window);
    }
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


      //  Gfx::Line line = {
     //       {0xffff0000, 10,20, 1},
     //       {0xffff0000, 20,40, 1}
     //   };

    
       // Gfx::Triangle triangle = {
        //    {0xffff0000, 10,20, 1},
         //   {0xffff0000, 20,40, 1}, 
          //  {0xffff0000, 40,50, 1}
        //};
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
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(*prims), *prims, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void *)sizeof(Color));
    glEnableVertexAttribArray(0);
    // color attribute
    //todo fix color, works with float but not unsigned int
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*)0);
    glEnableVertexAttribArray(1);

    glUseProgram(shaderProgram);
    setUniformVar<int>(shaderProgram, "scr_width", width);
    setUniformVar<int>(shaderProgram, "scr_height", height);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3*count);
}

void OpenGLRenderer::drawTexturedTriangles(const TexturedTriangle *prims, size_t count) {
    // TODO
}

}

#endif
