#include "renderer.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "file.hpp"
#ifndef PSP
#include "pc/glad.h"
#endif
#include "pc/stb_image.h"

namespace FS {
GFX::Texture loadTexFile(const std::string& path) {
    #ifndef PSP
    GFX::Texture tex;

    int channels;
    unsigned char* data = stbi_load(path.c_str(), &tex.width, &tex.height, &channels, 4);
    assert(data && "Failed to load texture");

    glGenTextures(1, &tex.id);
    glBindTexture(GL_TEXTURE_2D, tex.id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload RGBA data
    // Flip V if necessary: OpenGL expects bottom-left origin
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width, tex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    return tex;
    #endif
    return GFX::Texture();
}
}