#pragma once

#include <string>

#if PLATFORM_DESKTOP
    #include <glad/gl.h>
#elif PLATFORM_ANDROID
    // On Android, we need both EGL and GLES headers
    #include <glad/egl.h>
    #include <glad/gles2.h>
#elif PLATFORM_EMSCRIPTEN || PLATFORM_IOS
    #include <glad/gles2.h>
#endif

namespace Base {

class Texture {
public:
    Texture();
    ~Texture();

    // Loads a texture from a file.
    // Flips the image vertically to match OpenGL's coordinate system.
    bool loadFromFile(const std::string& path);

    // Binds the texture to a specific texture unit.
    void bind(GLuint textureUnit = 0) const;

    // Unbinds the texture from the given texture unit.
    void unbind(GLuint textureUnit = 0) const;

    // Gets the OpenGL texture ID.
    GLuint getID() const { return m_ID; }
    int getWidth() const { return m_Width; }
    int getHeight() const { return m_Height; }

    // Non-copyable
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

private:
    GLuint m_ID = 0;
    int m_Width = 0;
    int m_Height = 0;
    int m_NrChannels = 0;
};

} // namespace Base